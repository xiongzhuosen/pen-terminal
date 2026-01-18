#include "ssh_vnc_full.h"
#include <magic.h>
#include <md4c.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

// 多格式文件查看
int file_view(const char* file_path, char* content, int max_len) {
    if(!file_path || !content || max_len <=0) return -1;

    const char* type = file_get_type(file_path);
    FILE* fp = fopen(file_path, "r");
    if(!fp) return -1;

    memset(content, 0, max_len);
    // 纯文本文件：txt/log/conf/json
    if(strstr(type, "text") || strstr(type, "JSON") || strstr(type, "conf")) {
        fread(content, 1, max_len-1, fp);
    }
    // MD文件：先读取再渲染
    else if(strstr(type, "markdown") || strstr(file_path, ".md") || strstr(file_path, ".MD")) {
        char md_buf[4096] = {0};
        fread(md_buf, 1, sizeof(md_buf)-1, fp);
        md_render(md_buf, content, max_len);
    }
    // 二进制文件：显示十六进制（简易预览）
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

// MD格式渲染核心（基于md4c库）
void md_render(const char* md_content, char* html_content, int max_len) {
    if(!md_content || !html_content || max_len <=0) return;

    MD_PARSER parser;
    md_parser_init(&parser, MD_FLAG_NONE, NULL, NULL);
    // 渲染MD为格式化文本（适配嵌入式终端显示）
    md_parse(&parser, md_content, strlen(md_content), html_content, max_len);
    md_parser_free(&parser);
}
