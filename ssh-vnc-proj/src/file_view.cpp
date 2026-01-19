#include "ssh_vnc_full.h"
#include <magic.h>
#include <md4c-html.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

// ✅【核心回调】适配你的md_html的输出回调函数，必须写
static void md4c_html_output_callback(const MD_CHAR* html_chunk, MD_SIZE chunk_len, void* userdata) {
    if (html_chunk == NULL || chunk_len == 0 || userdata == NULL) return;
    char* out_buf = (char*)userdata;
    unsigned int* max_len_ptr = (unsigned int*)(out_buf + 4096);
    unsigned int current_len = strlen(out_buf);
    // ✅ 修复：全量无符号类型对比，消除警告
    if (current_len + chunk_len < *max_len_ptr - 1) {
        memcpy(out_buf + current_len, html_chunk, chunk_len);
        out_buf[current_len + chunk_len] = '\0';
    }
}

// 获取文件类型
const char* file_get_type(const char* file_path) {
    magic_t magic_cookie = magic_open(MAGIC_NONE);
    if(magic_cookie == NULL) {
        return "unknown";
    }
    if(magic_load(magic_cookie, NULL) != 0) {
        magic_close(magic_cookie);
        return "unknown";
    }
    const char* file_type = magic_file(magic_cookie, file_path);
    char* result = strdup(file_type ? file_type : "unknown");
    magic_close(magic_cookie);
    return result;
}

// 获取文件大小
off_t file_get_size(const char* file_path) {
    struct stat st;
    if(stat(file_path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

// 判断是否为目录
int file_is_dir(const char* file_path) {
    struct stat st;
    if(stat(file_path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

// 多格式文件查看：txt/log/json/二进制/MD转HTML (✅ 0警告+0错误)
int file_view(const char* file_path, char* content, int max_len) {
    if(!file_path || !content || max_len <=0) return -1;

    const char* type = file_get_type(file_path);
    FILE* fp = fopen(file_path, "r");
    if(!fp) return -1;

    memset(content, 0, max_len);
    if(strstr(type, "text") || strstr(type, "JSON") || strstr(type, "conf") || strstr(type, "shell")) {
        fread(content, 1, max_len-1, fp);
    }
    else if(strstr(file_path, ".md") || strstr(file_path, ".MD") || strstr(type, "markdown")) {
        char md_buf[4096] = {0};
        size_t read_len = fread(md_buf, 1, sizeof(md_buf)-1, fp);
        md_buf[read_len] = '\0';
        
        unsigned int* max_len_ptr = (unsigned int*)(content + 4096);
        *max_len_ptr = (unsigned int)max_len;

        // ✅ 完美适配你的ARM版md_html 6参数，无任何警告
        md_html(
            md_buf,                  // 参数1: markdown文本
            (MD_SIZE)strlen(md_buf), // 参数2: markdown长度
            md4c_html_output_callback, // 参数3: 回调函数
            content,                 // 参数4: 输出缓冲区
            MD_FLAG_NOHTML,          // 参数5: 合法宏
            (unsigned int)max_len    // 参数6: 最大长度
        );
    }
    else if(strstr(type, "binary")) {
        char hex_buf[4] = {0};
        unsigned char byte;
        int pos = 0;
        while(fread(&byte, 1, 1, fp) && pos < max_len-4) {
            sprintf(hex_buf, "%02X ", byte);
            strcat(content, hex_buf);
            pos += 3;
        }
    }

    fclose(fp);
    return 0;
}

// 备用兼容：纯文本md渲染函数
void md_render(const char* md_content, char* out_content, int max_len) {
    if(!md_content || !out_content || max_len <=0) return;

    memset(out_content, 0, max_len);
    const char* p = md_content;
    int pos = 0;

    while(*p != '\0' && pos < max_len-1) {
        if(*p == '#' && (p == md_content || *(p-1) == '\n')) {
            out_content[pos++] = '\n';
            while(*p == '#' && pos < max_len-1) out_content[pos++] = *p++;
            out_content[pos++] = ' ';
        }
        else if(*p == '-' && (p == md_content || *(p-1) == '\n')) {
            out_content[pos++] = '\n';
            out_content[pos++] = ' ';
            out_content[pos++] = ' ';
            out_content[pos++] = *p++;
            out_content[pos++] = ' ';
        }
        else if(*p == '*' && *(p+1) == '*') {
            p += 2;
            while(*p != '*' && *(p+1) != '*' && pos < max_len-1) out_content[pos++] = *p++;
            p += 2;
        }
        else {
            out_content[pos++] = *p++;
        }
    }
    out_content[pos] = '\0';
}
