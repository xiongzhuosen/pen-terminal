#include "include/ssh_conn_manager.h"
#include "include/vnc_input.h"
#include "include/file_ops.h"
#include <stdlib.h>
#include <string.h>

// 适配前端：内部分配固定buf（8192足够）
#define API_BUF_SIZE 8192
#define API_MALLOC(size) malloc(size)
#define API_FREE(ptr) free(ptr)

// 工具：分配buf并返回（前端框架自动释放）
static char* alloc_api_buf() {
    return (char*)API_MALLOC(API_BUF_SIZE);
}

extern "C" {

// ====================== SSH 导出（1:1匹配前端$api.ssh_xxx） ======================
int ssh_global_init() {
    return ::ssh_global_init_impl();
}

int ssh_connect(const char* ip, const char* port, const char* user, const char* pass) {
    return ::ssh_connect_impl(ip, port, user, pass);
}

int ssh_connect_key(const char* ip, const char* port, const char* user, const char* key_path) {
    return ::ssh_connect_key_impl(ip, port, user, key_path);
}

void ssh_disconnect() {
    ::ssh_disconnect_impl();
}

int ssh_exec(const char* cmd) {
    return ::ssh_write_stream_impl(cmd);
}

char* ssh_read_stream() {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::ssh_read_stream_impl(buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

int ssh_send_key(const char* key) {
    return ::ssh_send_key_impl(key);
}

int ssh_send_esc() {
    return ::ssh_send_esc_impl();
}

// ====================== VNC 导出（1:1匹配前端$api.vnc_xxx） ======================
int vnc_connect(const char* ip, const char* port, const char* pass) {
    return ::vnc_connect_impl(ip, port, pass);
}

void vnc_disconnect() {
    ::vnc_disconnect_impl();
}

char* vnc_read_frame() {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::vnc_read_frame_impl(buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

int vnc_send_mouse(const char* evt_json) {
    return ::vnc_send_mouse_impl(evt_json);
}

int vnc_send_key(const char* key) {
    return ::vnc_send_key_impl(key);
}

void vnc_set_scale(float scale) {
    ::vnc_set_scale_impl(scale);
}

// ====================== 文件 导出（1:1匹配前端$api.file_xxx） ======================
char* file_list(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int ret = ::file_list_impl(path, buf, API_BUF_SIZE - 1);
    if (ret != 0) { API_FREE(buf); return NULL; }
    return buf;
}

char* file_list_via_ssh(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int ret = ::file_list_via_ssh_impl(path, buf, API_BUF_SIZE - 1);
    if (ret != 0) { API_FREE(buf); return NULL; }
    return buf;
}

char* file_read_text(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::file_read_text_impl(path, buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char* file_read_hex(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::file_read_hex_impl(path, buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char* file_render_md(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::file_render_md_impl(path, buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

int file_write(const char* path, const char* content) {
    return ::file_write_impl(path, content);
}

int file_chmod(const char* path, const char* mode) {
    return ::file_chmod_impl(path, mode);
}

int file_chown(const char* path, const char* owner) {
    return ::file_chown_impl(path, owner);
}

char* file_lsattr(const char* path) {
    char* buf = alloc_api_buf();
    if (!buf) return NULL;
    int len = ::file_lsattr_impl(path, buf, API_BUF_SIZE - 1);
    if (len <= 0) { API_FREE(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

int file_delete(const char* path) {
    return ::file_delete_impl(path);
}

int file_rename(const char* old_path, const char* new_path) {
    return ::file_rename_impl(old_path, new_path);
}

int file_mkdir(const char* path) {
    return ::file_mkdir_impl(path);
}

} // extern "C"
