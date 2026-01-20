#include "include/ssh_conn_manager.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

static int sock_fd = -1;
static LIBSSH2_SESSION* ssh_session = nullptr;
static LIBSSH2_CHANNEL* ssh_channel = nullptr;

// ✅ 函数名加 _impl 后缀
int ssh_global_init_impl() {
    return libssh2_init(0);
}

int ssh_connect_impl(const char* ip, const char* port, const char* user, const char* pass) {
    if (ssh_session != nullptr) {
        ssh_disconnect_impl();
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) return -1;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        close(sock_fd);
        sock_fd = -1;
        return -2;
    }

    ssh_session = libssh2_session_init();
    if (ssh_session == nullptr) {
        close(sock_fd);
        sock_fd = -1;
        return -3;
    }

    libssh2_session_set_blocking(ssh_session, 0);
    if (libssh2_session_handshake(ssh_session, sock_fd) != 0) {
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -4;
    }

    if (libssh2_userauth_password(ssh_session, user, pass) != 0) {
        libssh2_session_disconnect(ssh_session, "Auth Failed");
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -5;
    }

    ssh_channel = libssh2_channel_open_session(ssh_session);
    if (ssh_channel == nullptr) {
        libssh2_session_disconnect(ssh_session, "Channel Open Failed");
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -6;
    }

    libssh2_channel_request_pty(ssh_channel, "xterm");
    libssh2_channel_shell(ssh_channel);

    return 0;
}

int ssh_connect_with_key_impl(const char* ip, const char* port, const char* user, const char* key_path) {
    if (ssh_session != nullptr) {
        ssh_disconnect_impl();
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) return -1;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        close(sock_fd);
        sock_fd = -1;
        return -2;
    }

    ssh_session = libssh2_session_init();
    if (ssh_session == nullptr) {
        close(sock_fd);
        sock_fd = -1;
        return -3;
    }

    libssh2_session_set_blocking(ssh_session, 0);
    if (libssh2_session_handshake(ssh_session, sock_fd) != 0) {
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -4;
    }

    if (libssh2_userauth_publickey_fromfile(ssh_session, user, nullptr, key_path, nullptr) != 0) {
        libssh2_session_disconnect(ssh_session, "Key Auth Failed");
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -5;
    }

    ssh_channel = libssh2_channel_open_session(ssh_session);
    if (ssh_channel == nullptr) {
        libssh2_session_disconnect(ssh_session, "Channel Open Failed");
        libssh2_session_free(ssh_session);
        close(sock_fd);
        ssh_session = nullptr;
        sock_fd = -1;
        return -6;
    }

    libssh2_channel_request_pty(ssh_channel, "xterm");
    libssh2_channel_shell(ssh_channel);

    return 0;
}

void ssh_disconnect_impl() {
    if (ssh_channel != nullptr) {
        libssh2_channel_close(ssh_channel);
        libssh2_channel_free(ssh_channel);
        ssh_channel = nullptr;
    }

    if (ssh_session != nullptr) {
        libssh2_session_disconnect(ssh_session, "Normal Shutdown");
        libssh2_session_free(ssh_session);
        ssh_session = nullptr;
    }

    if (sock_fd != -1) {
        close(sock_fd);
        sock_fd = -1;
    }
}

int ssh_write_stream_impl(const char* data) {
    if (ssh_channel == nullptr) return -1;
    return libssh2_channel_write(ssh_channel, data, strlen(data));
}

int ssh_read_stream_impl(char* buf, int buf_len) {
    if (ssh_channel == nullptr || buf == nullptr || buf_len <= 0) return -1;
    return libssh2_channel_read(ssh_channel, buf, buf_len - 1);
}
