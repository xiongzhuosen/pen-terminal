#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ✅ 实现函数加 _impl 后缀
int file_list_impl(const char* path, char* buf, int buf_len);
int file_list_via_ssh_impl(const char* path, char* buf, int buf_len);
int file_read_text_impl(const char* path, char* buf, int buf_len);
int file_read_hex_impl(const char* path, char* buf, int buf_len);
int file_render_md_impl(const char* path, char* buf, int buf_len);
int file_write_impl(const char* path, const char* content);
int file_chmod_impl(const char* path, const char* mode);
int file_chown_impl(const char* path, const char* user);
int file_lsattr_impl(const char* path, char* buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif
