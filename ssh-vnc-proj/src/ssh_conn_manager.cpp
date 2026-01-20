#ifndef SSH_CONN_MANAGER_H
#define SSH_CONN_MANAGER_H

#include <libssh2.h>
#include <string>
#include <vector>

// ✅ 添加 extern "C" 条件编译
#ifdef __cplusplus
extern "C" {
#endif

// 连接配置结构体
typedef struct {
    std::string ip;
    std::string port;
    std::string user;
    std::string pass;
    std::string key_path;
} SSHConn;

// 初始化SSH
int ssh_global_init();

// 密码登录
int ssh_connect(const char* ip, const char* port, const char* user, const char* pass);

// 密钥登录
int ssh_connect_with_key(const char* ip, const char* port, const char* user, const char* key_path);

// 断开连接
void ssh_disconnect();

// 流式写（支持vim/passwd）
int ssh_write_stream(const char* data);

// 流式读
int ssh_read_stream(char* buf, int buf_len);

// ✅ 闭合 extern "C"
#ifdef __cplusplus
}
#endif

#endif
