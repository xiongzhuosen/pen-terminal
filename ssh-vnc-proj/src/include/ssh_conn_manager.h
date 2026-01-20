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

// ✅ 实现函数加 _impl 后缀
int ssh_global_init_impl();
int ssh_connect_impl(const char* ip, const char* port, const char* user, const char* pass);
int ssh_connect_with_key_impl(const char* ip, const char* port, const char* user, const char* key_path);
void ssh_disconnect_impl();
int ssh_write_stream_impl(const char* data);
int ssh_read_stream_impl(char* buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif
