#ifndef SSH_CONN_MANAGER_H
#define SSH_CONN_MANAGER_H

#include <libssh2.h>

typedef struct {
    char ip[64];
    char port[8];
    char user[32];
    char pass[64];
    char key_path[128];
} SSHConn;

#ifdef __cplusplus
extern "C" {
#endif

// 原有_impl函数（不变）
int ssh_global_init_impl();
// ✅ 修正：ssh_connect_with_key_impl → ssh_connect_key_impl（匹配前端）
int ssh_connect_impl(const char* ip, const char* port, const char* user, const char* pass);
int ssh_connect_key_impl(const char* ip, const char* port, const char* user, const char* key_path);
void ssh_disconnect_impl();
int ssh_write_stream_impl(const char* data);
int ssh_read_stream_impl(char* buf, int buf_len);

// ✅ 补前端缺失的_impl函数声明
int ssh_send_key_impl(const char* key);
int ssh_send_esc_impl();

#ifdef __cplusplus
}
#endif

#endif