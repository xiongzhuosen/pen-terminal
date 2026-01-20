#include "include/ssh_conn_manager.h"
#include "include/vnc_input.h"
#include "include/file_ops.h"

// ✅ 导出函数：调用带 _impl 后缀的实现函数
extern "C" {

int ssh_global_init() {
    return ::ssh_global_init_impl();
}

int ssh_connect(const char* ip, const char* port, const char* user, const char* pass) {
    return ::ssh_connect_impl(ip, port, user, pass);
}

int ssh_connect_with_key(const char* ip, const char* port, const char* user, const char* key_path) {
    return ::ssh_connect_with_key_impl(ip, port, user, key_path);
}

void ssh_disconnect() {
    ::ssh_disconnect_impl();
}

int ssh_write_stream(const char* data) {
    return ::ssh_write_stream_impl(data);
}

int ssh_read_stream(char* buf, int buf_len) {
    return ::ssh_read_stream_impl(buf, buf_len);
}

int vnc_connect(const char* ip, const char* port, const char* pass) {
    return ::vnc_connect_impl(ip, port, pass);
}

void vnc_disconnect() {
    ::vnc_disconnect_impl();
}

void vnc_set_scale(float scale) {
    ::vnc_set_scale_impl(scale);
}

int vnc_read_frame(char* buf, int buf_len) {
    return ::vnc_read_frame_impl(buf, buf_len);
}

int vnc_send_input(const char* evt_json) {
    return ::vnc_send_input_impl(evt_json);
}

int vnc_send_key(const char* key) {
    return ::vnc_send_key_impl(key);
}

int file_list(const char* path, char* buf, int buf_len) {
    return ::file_list_impl(path, buf, buf_len);
}

int file_list_via_ssh(const char* path, char* buf, int buf_len) {
    return ::file_list_via_ssh_impl(path, buf, buf_len);
}

int file_read_text(const char* path, char* buf, int buf_len) {
    return ::file_read_text_impl(path, buf, buf_len);
}

int file_read_hex(const char* path, char* buf, int buf_len) {
    return ::file_read_hex_impl(path, buf, buf_len);
}

int file_render_md(const char* path, char* buf, int buf_len) {
    return ::file_render_md_impl(path, buf, buf_len);
}

int file_write(const char* path, const char* content) {
    return ::file_write_impl(path, content);
}

int file_chmod(const char* path, const char* mode) {
    return ::file_chmod_impl(path, mode);
}

int file_chown(const char* path, const char* user) {
    return ::file_chown_impl(path, user);
}

int file_lsattr(const char* path, char* buf, int buf_len) {
    return ::file_lsattr_impl(path, buf, buf_len);
}

} // extern "C"
