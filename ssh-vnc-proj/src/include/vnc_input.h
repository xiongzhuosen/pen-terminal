#ifndef VNC_INPUT_H
#define VNC_INPUT_H

#include <rfb/rfbclient.h>

#ifdef __cplusplus
extern "C" {
#endif

// ✅ 实现函数加 _impl 后缀
int vnc_connect_impl(const char* ip, const char* port, const char* pass);
void vnc_disconnect_impl();
void vnc_set_scale_impl(float scale);
int vnc_read_frame_impl(char* buf, int buf_len);
int vnc_send_input_impl(const char* evt_json);
int vnc_send_key_impl(const char* key);

#ifdef __cplusplus
}
#endif

#endif
