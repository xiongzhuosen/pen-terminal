#include "ssh_vnc_full.h"
#include <magic.h>
#include <md4c.h>
#include <md4c-html.h>  // ✅ 引入HTML扩展库头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

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

// 多格式文件查看：txt/log/json/二进制/MD转HTML（完整版功能）
int file_view(const char* file_path, char* content, int max_len) {
    if(!file_path || !content || max_len <=0) return -1;

    const char* type = file_get_type(file_path);
    FILE* fp = fopen(file_path, "r");
    if(!fp) return -1;

    memset(content, 0, max_len);
    // 1. 纯文本文件：直接读取
    if(strstr(type, "text") || strstr(type, "JSON") || strstr(type, "conf") || strstr(type, "shell")) {
        fread(content, 1, max_len-1, fp);
    }
    // 2. MD文件：调用libmd4c-html原生API转HTML，支持所有语法
    else if(strstr(file_path, ".md") || strstr(file_path, ".MD") || strstr(type, "markdown")) {
        char md_buf[4096] = {0};
        size_t read_len = fread(md_buf, 1, sizeof(md_buf)-1, fp);
        md_buf[read_len] = '\0';
        // ✅ 核心：libmd4c-html的原生渲染函数，一步转HTML
        md_html(md_buf, strlen(md_buf), content, max_len, MD_FLAG_NONE, NULL);
    }
    // 3. 二进制文件：十六进制预览
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

// 备用：极简纯文本MD渲染（无HTML依赖，适配嵌入式终端显示）
void md_render_simple(const char* md_content, char* out_content, int max_len) {
    if(!md_content || !out_content || max_len <=0) return;

    memset(out_content, 0, max_len);
    const char* p = md_content;
    int pos = 0;

    while(*p != '\0' && pos < max_len-1) {
        // 标题处理
        if(*p == '#' && (p == md_content || *(p-1) == '\n')) {
            out_content[pos++] = '\n';
            while(*p == '#' && pos < max_len-1) out_content[pos++] = *p++;
            out_content[pos++] = ' ';
        }
        // 列表处理
        else if(*p == '-' && (p == md_content || *(p-1) == '\n')) {
            out_content[pos++] = '\n';
            out_content[pos++] = ' ';
            out_content[pos++] = ' ';
            out_content[pos++] = *p++;
            out_content[pos++] = ' ';
        }
        // 加粗处理
        else if(*p == '*' && *(p+1) == '*') {
            p += 2;
            while(*p != '*' && *(p+1) != '*' && pos < max_len-1) out_content[pos++] = *p++;
            p += 2;
        }
        // 普通字符
        else {
            out_content[pos++] = *p++;
        }
    }
    out_content[pos] = '\0';
}