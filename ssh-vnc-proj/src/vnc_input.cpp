#include "include/vnc_input.h"
#include <jsoncpp/json/json.h>  // ✅ 替换 cjson 为 jsoncpp
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static rfbClient* client = nullptr;
static float scale = 1.0;
static char frame_buf[4096] = {0};

// ✅ 函数名加 _impl 后缀
int vnc_connect_impl(const char* ip, const char* port, const char* pass) {
    if (client) vnc_disconnect_impl();
    client = rfbGetClient(8, 3, 4);
    client->serverHost = (char*)ip;
    client->serverPort = atoi(port);
    client->password = (char*)pass;
    if (!rfbInitClient(client, nullptr, nullptr)) return -1;
    return 0;
}

void vnc_disconnect_impl() {
    if (client) {
        rfbClientCleanup(client);
        client = nullptr;
    }
}

void vnc_set_scale_impl(float s) {
    scale = s;
}

int vnc_read_frame_impl(char* buf, int buf_len) {
    if (!client) return -1;
    rfbProcessEvents(client, 10);
    snprintf(frame_buf, sizeof(frame_buf), "data:image/png;base64,%s", "FRAME_DATA_HERE");
    int len = strlen(frame_buf);
    if (len > buf_len) len = buf_len;
    memcpy(buf, frame_buf, len);
    return len;
}

// ✅ 用 jsoncpp 解析事件，替代 cjson
int vnc_send_input_impl(const char* evt_json) {
    if (!client) return -1;

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(evt_json, root)) {
        return -2; // 解析失败
    }

    std::string type = root["type"].asString();
    int x = root["x"].asInt();
    int y = root["y"].asInt();

    if (type == "down") {
        rfbSendPointerEvent(client, x, y, rfbButton1Mask, 1);
    } else if (type == "up") {
        rfbSendPointerEvent(client, x, y, 0, 0);
    } else if (type == "move") {
        rfbSendPointerEvent(client, x, y, rfbButton1Mask, 1);
    }

    return 0;
}

int vnc_send_key_impl(const char* key) {
    if (!client) return -1;
    rfbSendKeyEvent(client, key[0], 1);
    usleep(1000);
    rfbSendKeyEvent(client, key[0], 0);
    return 0;
}
