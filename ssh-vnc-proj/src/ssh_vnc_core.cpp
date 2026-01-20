#include "include/ssh_conn_manager.h"
#include "include/vnc_input.h"
#include "include/file_ops.h"

// ✅ 直接导出函数，链接属性由头文件的 extern "C" 决定
extern "C" {

int ssh_global_init() {
    return ::ssh_global_init();
}

int ssh_connect(const char* ip, const char* port, const char* user, const char* pass) {
    return ::ssh_connect(ip, port, user, pass);
}

int ssh_connect_with_key(const char* ip, const char* port, const char* user, const char* key_path) {
    return ::ssh_connect_with_key(ip, port, user, key_path);
}

void ssh_disconnect() {
    ::ssh_disconnect();
}

int ssh_write_stream(const char* data) {
    return ::ssh_write_stream(data);
}

int ssh_read_stream(char* buf, int buf_len) {
    return ::ssh_read_stream(buf, buf_len);
}

int vnc_connect(const char* ip, const char* port, const char* pass) {
    return ::vnc_connect(ip, port, pass);
}

void vnc_disconnect() {
    ::vnc_disconnect();
}

void vnc_set_scale(float scale) {
    ::vnc_set_scale(scale);
}

int vnc_read_frame(char* buf, int buf_len) {
    return ::vnc_read_frame(buf, buf_len);
}

int vnc_send_input(const char* evt_json) {
    return ::vnc_send_input(evt_json);
}

int vnc_send_key(const char* key) {
    return ::vnc_send_key(key);
}

int file_list(const char* path, char* buf, int buf_len) {
    return ::file_list(path, buf, buf_len);
}

int file_list_via_ssh(const char* path, char* buf, int buf_len) {
    return ::file_list_via_ssh(path, buf, buf_len);
}

int file_read_text(const char* path, char* buf, int buf_len) {
    return ::file_read_text(path, buf, buf_len);
}

int file_read_hex(const char* path, char* buf, int buf_len) {
    return ::file_read_hex(path, buf, buf_len);
}

int file_render_md(const char* path, char* buf, int buf_len) {
    return ::file_render_md(path, buf, buf_len);
}

int file_write(const char* path, const char* content) {
    return ::file_write(path, content);
}

int file_chmod(const char* path, const char* mode) {
    return ::file_chmod(path, mode);
}

int file_chown(const char* path, const char* user) {
    return ::file_chown(path, user);
}

int file_lsattr(const char* path, char* buf, int buf_len) {
    return ::file_lsattr(path, buf, buf_len);
}

} // extern "C"
