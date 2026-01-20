#include "include/file_ops.h"
#include "include/ssh_conn_manager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include <md4c-html/md4c-html.h>

int file_list(const char* path, char* buf, int buf_len) {
    DIR* dir = opendir(path);
    if (!dir) return -1;
    cJSON* root = cJSON_CreateArray();
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        struct stat st;
        stat(entry->d_name, &st);
        cJSON* item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "name", entry->d_name);
        cJSON_AddNumberToObject(item, "size", st.st_size);
        cJSON_AddItemToArray(root, item);
    }
    closedir(dir);
    char* json_str = cJSON_Print(root);
    int len = strlen(json_str);
    if (len > buf_len) len = buf_len;
    memcpy(buf, json_str, len);
    free(json_str);
    cJSON_Delete(root);
    return 0;
}

int file_list_via_ssh(const char* path, char* buf, int buf_len) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls -l %s", path);
    ssh_write_stream(cmd);
    return ssh_read_stream(buf, buf_len);
}

int file_read_text(const char* path, char* buf, int buf_len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    int len = read(fd, buf, buf_len - 1);
    close(fd);
    buf[len] = '\0';
    return len;
}

int file_read_hex(const char* path, char* buf, int buf_len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    unsigned char data[1];
    int len = 0;
    while (read(fd, data, 1) > 0 && len < buf_len - 2) {
        snprintf(buf + len, 3, "%02x ", data[0]);
        len += 3;
    }
    close(fd);
    buf[len] = '\0';
    return len;
}

int file_render_md(const char* path, char* buf, int buf_len) {
    char md_content[8192];
    int len = file_read_text(path, md_content, sizeof(md_content));
    if (len < 0) return -1;
    md_html(md_content, len, buf, buf_len, MD_FLAG_NOHTML, nullptr, nullptr);
    return strlen(buf);
}

int file_write(const char* path, const char* content) {
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) return -1;
    int len = write(fd, content, strlen(content));
    close(fd);
    return len;
}

int file_chmod(const char* path, const char* mode) {
    return chmod(path, strtol(mode, nullptr, 8));
}

int file_chown(const char* path, const char* user) {
    // 解析user为uid/gid，实际需实现
    return 0;
}

int file_lsattr(const char* path, char* buf, int buf_len) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "lsattr %s", path);
    FILE* fp = popen(cmd, "r");
    if (!fp) return -1;
    int len = fread(buf, 1, buf_len - 1, fp);
    pclose(fp);
    buf[len] = '\0';
    return len;
}
