#include "ssh_vnc_full.h"
#include <libssh2.h>
#include <libssh2_publickey.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 全局变量定义
LIBSSH2_SESSION *g_ssh = NULL;
LIBSSH2_CHANNEL *g_channel = NULL;
DataCallback g_ssh_cb = NULL;

// 自定义快捷命令（可无限扩展，修改这里即可）
SshQuickCmd g_quick_cmds[] = {
    {"系统信息", "uname -a"},
    {"查看CPU", "top -b -n1 | head -10"},
    {"内存占用", "free -h"},
    {"磁盘信息", "df -h"},
    {"当前用户", "whoami && id"},
    {"清屏", "clear"},
    {"重启SSH", "systemctl restart sshd"},
    {"自定义命令", "ls -l /root"}
};
int g_quick_cmd_count = sizeof(g_quick_cmds)/sizeof(g_quick_cmds[0]);

// SSH初始化：带PTY伪终端，支持vim/passwd等交互命令
int ssh_init(const char* params) {
    int rc = libssh2_init(0);
    if (rc != 0) return -1;

    g_ssh = libssh2_session_init();
    if(!g_ssh) return -1;

    // 申请PTY伪终端（核心！交互命令必备）
    rc = libssh2_channel_request_pty(g_ssh, "xterm", 80, 24, 0, 0, NULL);
    if(rc != 0) {
        libssh2_session_free(g_ssh);
        return -1;
    }

    g_channel = libssh2_channel_open_session(g_ssh);
    libssh2_channel_setenv(g_channel, "TERM", "xterm");
    libssh2_channel_request_shell(g_channel); // 开启交互式shell
    return g_channel ? 0 : -1;
}

// 绑定流式数据回调
void ssh_attach_stream(DataCallback cb) { g_ssh_cb = cb; }

// 发送输入到SSH终端
int ssh_send_input(const char* data) {
    if(g_channel && data) return libssh2_channel_write(g_channel, data, strlen(data));
    return -1;
}

// 读取SSH流式数据（终端回显核心）
int ssh_read_stream(char* buf, int len) {
    if(!g_channel || !buf || len <=0) return -1;
    return libssh2_channel_read(g_channel, buf, len);
}

// 执行快捷命令（按命令字符串）
int ssh_exec_quick_cmd(const char* cmd) {
    if(!cmd) return -1;
    ssh_send_input(cmd);
    ssh_send_input("\n");
    return 0;
}

// 执行快捷命令（按按钮序号）
int ssh_exec_quick_cmd_by_idx(int idx) {
    if(idx <0 || idx >= g_quick_cmd_count) return -1;
    return ssh_exec_quick_cmd(g_quick_cmds[idx].cmd_str);
}

// 执行shell命令
int ssh_exec_shell_cmd(const char* cmd) {
    if(g_channel) return libssh2_channel_exec(g_channel, cmd);
    return -1;
}

// 关闭SSH连接
void ssh_close(void) {
    if(g_channel) libssh2_channel_close(g_channel);
    if(g_ssh) {
        libssh2_session_disconnect(g_ssh, "Normal Shutdown");
        libssh2_session_free(g_ssh);
    }
    libssh2_exit();
}

// 清屏
int ssh_clear_terminal(void) {
    if(g_ssh_cb) g_ssh_cb("\033[H\033[2J", 7, 3);
    return 0;
}
