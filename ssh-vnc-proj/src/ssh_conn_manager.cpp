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

// 原有_impl函数（仅修正ssh_connect_with_key_impl→ssh_connect_key_impl）
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

// ✅ 修正：ssh_connect_with_key_impl → ssh_connect_key_impl
int ssh_connect_key_impl(const char* ip, const char* port, const char* user, const char* key_path) {
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

// ✅ 补实现：ssh_send_key_impl（发送SSH按键，支持esc/方向键等）
int ssh_send_key_impl(const char* key) {
    if (ssh_channel == nullptr || !key) return -1;
    char key_seq[16] = {0};

    // 映射特殊按键到SSH终端序列
    if (strcmp(key, "esc") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c", 27); // ESC
    } else if (strcmp(key, "up") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c[%c", 27, 'A'); // ↑
    } else if (strcmp(key, "down") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c[%c", 27, 'B'); // ↓
    } else if (strcmp(key, "left") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c[%c", 27, 'D'); // ←
    } else if (strcmp(key, "right") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c[%c", 27, 'C'); // →
    } else if (strcmp(key, "tab") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c", 9); // Tab
    } else if (strcmp(key, "enter") == 0) {
        snprintf(key_seq, sizeof(key_seq), "%c", 10); // Enter
    } else {
        // 普通字符
        snprintf(key_seq, sizeof(key_seq), "%s", key);
    }

    return libssh2_channel_write(ssh_channel, key_seq, strlen(key_seq));
}

// ✅ 补实现：ssh_send_esc_impl（发送ESC，vim必备）
int ssh_send_esc_impl() {
    return ssh_send_key_impl("esc");
}