#ifndef VNC_INPUT_H
#define VNC_INPUT_H

#include <rfb/rfbclient.h>

#ifdef __cplusplus
extern "C" {
#endif

int vnc_connect_impl(const char* ip, const char* port, const char* pass);
void vnc_disconnect_impl();
void vnc_set_scale_impl(float scale);
int vnc_read_frame_impl(char* buf, int buf_len);
// ✅ 修正：vnc_send_input_impl → vnc_send_mouse_impl（匹配前端）
int vnc_send_mouse_impl(const char* evt_json);
int vnc_send_key_impl(const char* key);

#ifdef __cplusplus
}
#endif

#endif