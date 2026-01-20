#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <string>

// ✅ 添加 extern "C" 条件编译
#ifdef __cplusplus
extern "C" {
#endif

// 列出目录文件
int file_list(const char* path, char* buf, int buf_len);

// 通过SSH列出文件
int file_list_via_ssh(const char* path, char* buf, int buf_len);

// 读取文本内容
int file_read_text(const char* path, char* buf, int buf_len);

// 读取16进制内容
int file_read_hex(const char* path, char* buf, int buf_len);

// MD渲染
int file_render_md(const char* path, char* buf, int buf_len);

// 写入文件
int file_write(const char* path, const char* content);

// 修改权限
int file_chmod(const char* path, const char* mode);

// 修改属主
int file_chown(const char* path, const char* user);

// 查看属性
int file_lsattr(const char* path, char* buf, int buf_len);

// ✅ 闭合 extern "C"
#ifdef __cplusplus
}
#endif

#endif
