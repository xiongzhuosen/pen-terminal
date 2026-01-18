#ifndef SSH_VNC_FULL_H
#define SSH_VNC_FULL_H

#include <stdint.h>
#include <libssh2.h>
#include <rfb/rfbclient.h>
#include <magic.h>
#include <md4c.h>

// 回调函数定义：流式数据传输
typedef void (*DataCallback)(const char* data, int len, int type);

// 快捷命令结构体定义
typedef struct {
    char cmd_name[32];  // 按钮显示名称
    char cmd_str[128];  // 执行的命令
} SshQuickCmd;

// ======================== 全局变量跨文件声明（SSH+VNC） ========================
// SSH全局变量
extern LIBSSH2_SESSION *g_ssh;
extern LIBSSH2_CHANNEL *g_channel;
extern DataCallback g_ssh_cb;
extern SshQuickCmd g_quick_cmds[];
extern int g_quick_cmd_count;

// VNC全局变量
extern rfbClient *g_vnc;
extern int g_vnc_w;
extern int g_vnc_h;
extern DataCallback g_vnc_cb;

// ======================== SSH 核心功能（流式终端+快捷命令） ========================
int ssh_init(const char* params);
void ssh_attach_stream(DataCallback cb);
int ssh_send_input(const char* data);
int ssh_read_stream(char* buf, int len);
int ssh_exec_quick_cmd(const char* cmd);
int ssh_exec_quick_cmd_by_idx(int idx);
int ssh_exec_shell_cmd(const char* cmd);
void ssh_close(void);
int ssh_clear_terminal(void);

// ======================== VNC 核心功能（对标RVNC Viewer） ========================
int vnc_init(const char* params);
void vnc_attach_frame(DataCallback cb);
int vnc_send_mouse(const char* params);
int vnc_send_key(int key_code);
void vnc_resize(int w, int h);
void vnc_set_quality(int level); // 画质调节
int vnc_clipboard_sync(const char* text); // 剪贴板共享
void vnc_close(void);
int vnc_keep_alive(void);

// ======================== 文件管理 全功能（lsattr/chmod/chown/赋权） ========================
int file_upload(const char* local_path, const char* remote_path);
int file_download(const char* remote_path, const char* local_path);
int file_chmod(const char* file_path, mode_t mode);
int file_chown(const char* file_path, uid_t uid, gid_t gid);
int file_lsattr(const char* file_path, char* buf, int len);
int file_chattr(const char* file_path, const char* attr);
int file_is_dir(const char* file_path);
off_t file_get_size(const char* file_path);
const char* file_get_type(const char* file_path);

// ======================== 文件查看+MD渲染 全功能 ========================
int file_view(const char* file_path, char* content, int max_len);
void md_render(const char* md_content, char* html_content, int max_len);

// ======================== 内置软键盘 核心功能 ========================
int keyboard_init(void);
int keyboard_send_key(int key_code);
void keyboard_close(void);

#endif // SSH_VNC_FULL_H
