#ifndef VNC_INPUT_H
#define VNC_INPUT_H

#include <rfb/rfbclient.h>
#include <string>

// ✅ 添加 extern "C" 条件编译
#ifdef __cplusplus
extern "C" {
#endif

// VNC连接
int vnc_connect(const char* ip, const char* port, const char* pass);

// 断开VNC
void vnc_disconnect();

// 设置缩放比例
void vnc_set_scale(float scale);

// 读取帧数据
int vnc_read_frame(char* buf, int buf_len);

// 发送输入事件（键鼠）
int vnc_send_input(const char* evt_json);

// 发送键盘事件
int vnc_send_key(const char* key);

// ✅ 闭合 extern "C"
#ifdef __cplusplus
}
#endif

#endif
