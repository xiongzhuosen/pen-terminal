#include "ssh_vnc_full.h"
#include <libssh2_sftp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>  // ✅ 核心修复：补加缺失的string头文件，解决std::string报错

// 文件上传
int file_upload(const char* local_path, const char* remote_path) {
    if(!g_ssh || !local_path || !remote_path) return -1;

    LIBSSH2_SFTP *sftp_session = libssh2_sftp_init(g_ssh);
    if(!sftp_session) return -1;

    LIBSSH2_SFTP_HANDLE *handle = libssh2_sftp_open(sftp_session, remote_path, LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT, 0644);
    if(!handle) {
        libssh2_sftp_shutdown(sftp_session);
        return -1;
    }

    FILE *fp = fopen(local_path, "rb");
    if(!fp) {
        libssh2_sftp_close(handle);
        libssh2_sftp_shutdown(sftp_session);
        return -1;
    }

    char buf[4096];
    size_t nread;
    while((nread = fread(buf, 1, sizeof(buf), fp)) > 0) {
        libssh2_sftp_write(handle, buf, nread);
    }

    fclose(fp);
    libssh2_sftp_close(handle);
    libssh2_sftp_shutdown(sftp_session);
    return 0;
}

// 文件下载
int file_download(const char* remote_path, const char* local_path) {
    if(!g_ssh || !remote_path || !local_path) return -1;

    LIBSSH2_SFTP *sftp_session = libssh2_sftp_init(g_ssh);
    if(!sftp_session) return -1;

    LIBSSH2_SFTP_HANDLE *handle = libssh2_sftp_open(sftp_session, remote_path, LIBSSH2_FXF_READ, 0);
    if(!handle) {
        libssh2_sftp_shutdown(sftp_session);
        return -1;
    }

    FILE *fp = fopen(local_path, "wb");
    if(!fp) {
        libssh2_sftp_close(handle);
        libssh2_sftp_shutdown(sftp_session);
        return -1;
    }

    char buf[4096];
    ssize_t nread;
    while((nread = libssh2_sftp_read(handle, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, nread, fp);
    }

    fclose(fp);
    libssh2_sftp_close(handle);
    libssh2_sftp_shutdown(sftp_session);
    return 0;
}

// 修改文件权限 chmod
int file_chmod(const char* file_path, mode_t mode) {
    if(!file_path || access(file_path, F_OK) != 0) return -1;
    return chmod(file_path, mode);
}

// 修改文件属主/属组 chown
int file_chown(const char* file_path, uid_t uid, gid_t gid) {
    if(!file_path || access(file_path, F_OK) != 0) return -1;
    return chown(file_path, uid, gid);
}

// 查看文件扩展属性 lsattr
int file_lsattr(const char* file_path, char* buf, int len) {
    if(!file_path || !buf || len <=0) return -1;
    FILE* fp = popen((std::string("lsattr ") + file_path).c_str(), "r");
    if(!fp) return -1;
    fgets(buf, len, fp);
    pclose(fp);
    return 0;
}

// 修改文件扩展属性 chattr（赋权核心）
int file_chattr(const char* file_path, const char* attr) {
    if(!file_path || !attr) return -1;
    return system((std::string("chattr ") + attr + " " + file_path).c_str());
}

