#ifndef SSH_CONN_MANAGER_H
#define SSH_CONN_MANAGER_H

#include <libssh2.h>
// ✅ 移除 C++ 头文件 #include <string> #include <vector>

// 适配 C 语言的连接配置结构体（用 char 数组替代 std::string）
typedef struct {
    char ip[64];          // IP 地址，固定长度避免内存泄漏
    char port[8];         // 端口号（如 "22"）
    char user[32];        // 用户名
    char pass[64];        // 密码
    char key_path[128];   // 密钥文件路径
} SSHConn;

// ✅ 全局 extern "C" 包裹，强制所有函数使用 C 链接
#ifdef __cplusplus
extern "C" {
#endif

// 初始化 SSH 全局环境
int ssh_global_init();

// 密码方式登录 SSH
int ssh_connect(const char* ip, const char* port, const char* user, const char* pass);

// 密钥方式登录 SSH
int ssh_connect_with_key(const char* ip, const char* port, const char* user, const char* key_path);

// 断开 SSH 连接
void ssh_disconnect();

// 向 SSH 终端写入数据（流式）
int ssh_write_stream(const char* data);

// 从 SSH 终端读取数据（流式）
int ssh_read_stream(char* buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif
