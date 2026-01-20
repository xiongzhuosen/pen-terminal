#include "include/vnc_input.h"
#include <cjson/cJSON.h>
#include <unistd.h>

static rfbClient* client = nullptr;
static float scale = 1.0;
static char frame_buf[4096] = {0};

int vnc_connect(const char* ip, const char* port, const char* pass) {
    if (client) vnc_disconnect();
    client = rfbGetClient(8, 3, 4);
    client->serverHost = (char*)ip;
    client->serverPort = atoi(port);
    client->password = (char*)pass;
    if (!rfbInitClient(client, nullptr, nullptr)) return -1;
    return 0;
}

void vnc_disconnect() {
    if (client) { rfbClientCleanup(client); client = nullptr; }
}

void vnc_set_scale(float s) { scale = s; }

int vnc_read_frame(char* buf, int buf_len) {
    if (!client) return -1;
    rfbProcessEvents(client, 10);
    // 模拟帧数据（实际需从framebuffer读取并编码）
    snprintf(frame_buf, sizeof(frame_buf), "data:image/png;base64,%s", "FRAME_DATA_HERE");
    int len = strlen(frame_buf);
    if (len > buf_len) len = buf_len;
    memcpy(buf, frame_buf, len);
    return len;
}

int vnc_send_input(const char* evt_json) {
    if (!client) return -1;
    cJSON* root = cJSON_Parse(evt_json);
    if (!root) return -2;
    const char* type = cJSON_GetObjectItem(root, "type")->valuestring;
    int x = cJSON_GetObjectItem(root, "x")->valueint;
    int y = cJSON_GetObjectItem(root, "y")->valueint;
    // 处理鼠标事件
    if (strcmp(type, "down") == 0) rfbSendPointerEvent(client, x, y, rfbButton1Mask, 1);
    else if (strcmp(type, "up") == 0) rfbSendPointerEvent(client, x, y, 0, 0);
    else if (strcmp(type, "move") == 0) rfbSendPointerEvent(client, x, y, rfbButton1Mask, 1);
    cJSON_Delete(root);
    return 0;
}

int vnc_send_key(const char* key) {
    if (!client) return -1;
    // 转换key为键值并发送
    rfbSendKeyEvent(client, key[0], 1);
    usleep(1000);
    rfbSendKeyEvent(client, key[0], 0);
    return 0;
}
