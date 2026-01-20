#include "include/file_ops.h"
#include "include/ssh_conn_manager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include <md4c-html.h>
#include <string.h>
#include <stdlib.h>

static void md_output_callback(const MD_CHAR* data, MD_SIZE size, void* userdata) {
    char** ctx = (char**)userdata;
    char* buf = ctx[0];
    int* remain_len = (int*)ctx[1];

    if (size <= 0 || *remain_len <= 1) return;
    MD_SIZE write_len = size < (MD_SIZE)(*remain_len - 1) ? size : (MD_SIZE)(*remain_len - 1);
    memcpy(buf, data, write_len);
    
    ctx[0] += write_len;
    *remain_len -= write_len;
}

// ✅ 函数名加 _impl 后缀
int file_list_impl(const char* path, char* buf, int buf_len) {
    DIR* dir = opendir(path);
    if (!dir) return -1;

    Json::Value root(Json::arrayValue);
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string full_path = std::string(path) + "/" + entry->d_name;
        struct stat st;
        if (stat(full_path.c_str(), &st) != 0) continue;

        Json::Value item;
        item["name"] = entry->d_name;
        item["size"] = (Json::UInt64)st.st_size;
        root.append(item);
    }
    closedir(dir);

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    std::string json_str = Json::writeString(writer, root);
    
    int len = json_str.length();
    if (len > buf_len - 1) len = buf_len - 1;
    memcpy(buf, json_str.c_str(), len);
    buf[len] = '\0';
    return 0;
}

int file_list_via_ssh_impl(const char* path, char* buf, int buf_len) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls -l %s", path);
    ssh_write_stream_impl(cmd);
    return ssh_read_stream_impl(buf, buf_len);
}

int file_read_text_impl(const char* path, char* buf, int buf_len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    int len = read(fd, buf, buf_len - 1);
    close(fd);
    buf[len] = '\0';
    return len;
}

int file_read_hex_impl(const char* path, char* buf, int buf_len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    unsigned char data[1];
    int len = 0;

    while (read(fd, data, 1) > 0 && len < buf_len - 4) {
        snprintf(buf + len, 4, "%02x ", data[0]);
        len += 3;
    }
    close(fd);
    buf[len] = '\0';
    return len;
}

int file_render_md_impl(const char* path, char* buf, int buf_len) {
    if (buf == nullptr || buf_len <= 0) return -1;

    char md_content[8192] = {0};
    int content_len = file_read_text_impl(path, md_content, sizeof(md_content));
    if (content_len < 0) return -1;

    char* buf_ptr = buf;
    int remain_len = buf_len;
    char* ctx[] = {buf_ptr, (char*)&remain_len};

    md_html(
        md_content,
        content_len,
        md_output_callback,
        ctx,
        MD_FLAG_NOHTML,
        0
    );
    *buf_ptr = '\0';
    return strlen(buf);
}

int file_write_impl(const char* path, const char* content) {
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd < 0) return -1;
    int len = write(fd, content, strlen(content));
    close(fd);
    return len;
}

int file_chmod_impl(const char* path, const char* mode) {
    return chmod(path, strtol(mode, nullptr, 8));
}

int file_chown_impl(const char* path, const char* user) {
    return 0;
}

int file_lsattr_impl(const char* path, char* buf, int buf_len) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "lsattr %s", path);
    FILE* fp = popen(cmd, "r");
    if (!fp) return -1;

    int len = fread(buf, 1, buf_len - 1, fp);
    pclose(fp);
    buf[len] = '\0';
    return len;
}
