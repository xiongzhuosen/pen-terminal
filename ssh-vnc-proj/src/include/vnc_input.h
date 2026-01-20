#ifndef VNC_INPUT_H
#define VNC_INPUT_H

#include <rfb/rfbclient.h>
// ✅ 移除 C++ 头文件 #include <string>

// ✅ 全局 extern "C" 包裹
#ifdef __cplusplus
extern "C" {
#endif

// 连接 VNC 服务器
int vnc_connect(const char* ip, const char* port, const char* pass);

// 断开 VNC 连接
void vnc_disconnect();

// 设置 VNC 画面缩放比例
void vnc_set_scale(float scale);

// 读取 VNC 帧数据（base64 编码）
int vnc_read_frame(char* buf, int buf_len);

// 发送键鼠事件（JSON 格式字符串）
int vnc_send_input(const char* evt_json);

// 发送键盘按键事件
int vnc_send_key(const char* key);

#ifdef __cplusplus
}
#endif

#endif
