#include "ssh_vnc_full.h"
#include <stdlib.h>
#include <string.h>

// 内置键盘按键映射表：ASCII码 -> 键盘码
const int g_key_map[][2] = {
    // 字母
    {'a', 0x61}, {'b', 0x62}, {'c', 0x63}, {'d', 0x64}, {'e', 0x65},
    {'f', 0x66}, {'g', 0x67}, {'h', 0x68}, {'i', 0x69}, {'j', 0x6A},
    // 数字
    {'0', 0x30}, {'1', 0x31}, {'2', 0x32}, {'3', 0x33}, {'4', 0x34},
    {'5', 0x35}, {'6', 0x36}, {'7', 0x37}, {'8', 0x38}, {'9', 0x39},
    // 功能键（vim/passwd必备）
    {'\n', 0x0D}, {'\b', 0x08}, {27, 0x1B}, {' ', 0x20}, {'\t', 0x09}
};
int g_key_map_count = sizeof(g_key_map)/sizeof(g_key_map[0]);

// 键盘初始化
int keyboard_init(void) {
    // 初始化按键映射表
    return 0;
}

// 发送按键：自动适配SSH/VNC
int keyboard_send_key(int key_code) {
    // 发送到SSH终端
    if(g_ssh && g_channel) {
        char key = (char)key_code;
        libssh2_channel_write(g_channel, &key, 1);
    }
    // 发送到VNC服务端
    if(g_vnc) {
        vnc_send_key(key_code);
    }
    return 0;
}

// 关闭键盘
void keyboard_close(void) {
    // 空实现
}
