#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 原有_impl函数（不变）
int file_list_impl(const char* path, char* buf, int buf_len);
int file_list_via_ssh_impl(const char* path, char* buf, int buf_len);
int file_read_text_impl(const char* path, char* buf, int buf_len);
int file_read_hex_impl(const char* path, char* buf, int buf_len);
int file_render_md_impl(const char* path, char* buf, int buf_len);
int file_write_impl(const char* path, const char* content);
int file_chmod_impl(const char* path, const char* mode);
int file_chown_impl(const char* path, const char* user);
int file_lsattr_impl(const char* path, char* buf, int buf_len);

// ✅ 补前端缺失的_impl函数声明
int file_delete_impl(const char* path);
int file_rename_impl(const char* old_path, const char* new_path);
int file_mkdir_impl(const char* path);

#ifdef __cplusplus
}
#endif

#endif