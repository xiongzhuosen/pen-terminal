#include "include/ssh_conn_manager.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

static int sock = -1;
static LIBSSH2_SESSION* session = nullptr;
static LIBSSH2_CHANNEL* channel = nullptr;

int ssh_global_init() {
    return libssh2_init(0);
}

int ssh_connect(const char* ip, const char* port, const char* user, const char* pass) {
    if (session) ssh_disconnect();
    // 创建socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port));
    sin.sin_addr.s_addr = inet_addr(ip);
    if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0) return -1;
    // 创建session
    session = libssh2_session_init();
    libssh2_session_set_blocking(session, 0);
    if (libssh2_session_handshake(session, sock) != 0) return -2;
    // 密码认证
    if (libssh2_userauth_password(session, user, pass) != 0) return -3;
    // 创建channel
    channel = libssh2_channel_open_session(session);
    libssh2_channel_request_pty(channel, "xterm");
    libssh2_channel_shell(channel);
    return 0;
}

int ssh_connect_with_key(const char* ip, const char* port, const char* user, const char* key_path) {
    if (session) ssh_disconnect();
    // 同密码登录，替换为密钥认证
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port));
    sin.sin_addr.s_addr = inet_addr(ip);
    if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0) return -1;
    session = libssh2_session_init();
    libssh2_session_set_blocking(session, 0);
    if (libssh2_session_handshake(session, sock) != 0) return -2;
    // 密钥认证
    if (libssh2_userauth_publickey_fromfile(session, user, nullptr, key_path, nullptr) != 0) return -3;
    channel = libssh2_channel_open_session(session);
    libssh2_channel_request_pty(channel, "xterm");
    libssh2_channel_shell(channel);
    return 0;
}

void ssh_disconnect() {
    if (channel) { libssh2_channel_close(channel); libssh2_channel_free(channel); }
    if (session) { libssh2_session_disconnect(session, "Normal Shutdown"); libssh2_session_free(session); }
    if (sock != -1) { close(sock); sock = -1; }
}

int ssh_write_stream(const char* data) {
    if (!channel) return -1;
    return libssh2_channel_write(channel, data, strlen(data));
}

int ssh_read_stream(char* buf, int buf_len) {
    if (!channel) return -1;
    return libssh2_channel_read(channel, buf, buf_len);
}
