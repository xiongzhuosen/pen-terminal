#ifndef FILE_OPS_H
#define FILE_OPS_H

// ✅ 无 C++ 头文件依赖
#include <stdint.h>

// ✅ 全局 extern "C" 包裹
#ifdef __cplusplus
extern "C" {
#endif

// 列出指定路径下的文件（输出 JSON 字符串）
int file_list(const char* path, char* buf, int buf_len);

// 通过 SSH 列出文件（备用方案）
int file_list_via_ssh(const char* path, char* buf, int buf_len);

// 读取文件的文本内容
int file_read_text(const char* path, char* buf, int buf_len);

// 读取文件的 16 进制内容
int file_read_hex(const char* path, char* buf, int buf_len);

// 渲染 Markdown 文件为 HTML
int file_render_md(const char* path, char* buf, int buf_len);

// 写入内容到文件
int file_write(const char* path, const char* content);

// 修改文件权限（如 "755"）
int file_chmod(const char* path, const char* mode);

// 修改文件属主（如 "root:root"）
int file_chown(const char* path, const char* user);

// 查看文件扩展属性（调用 lsattr 命令）
int file_lsattr(const char* path, char* buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif
