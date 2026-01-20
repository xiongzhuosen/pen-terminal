#include "include/vnc_input.h"
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// 全局变量：存储VNC密码和客户端实例
static char vnc_pass[64] = {0};
static rfbClient* client = nullptr;
static float scale = 1.0;
// ✅ 模拟base64占位图（Vue可直接渲染，替换成你需要的占位图base64）
static const char* SIMULATE_FRAME_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAHBhaW50Lm5ldCA0LjAuNWWFMmUAAABoSURBVHhe7cExAQAAAMKg9U9tDQ+gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADwD5UAB6h9c4gAAAABJRU5ErkJggg==";

// VNC密码回调函数
static char* vnc_get_password(rfbClient* cl) {
    return vnc_pass;
}

int vnc_connect_impl(const char* ip, const char* port, const char* pass) {
    if (client) {
        vnc_disconnect_impl();
    }

    client = rfbGetClient(8, 3, 4);
    if (!client) {
        return -1;
    }

    client->serverHost = (char*)ip;
    client->serverPort = atoi(port);
    strncpy(vnc_pass, pass, sizeof(vnc_pass) - 1);
    client->GetPassword = vnc_get_password;

    if (!rfbInitClient(client, nullptr, nullptr)) {
        rfbClientCleanup(client);
        client = nullptr;
        return -2;
    }

    return 0;
}

void vnc_disconnect_impl() {
    if (client) {
        rfbClientCleanup(client);
        client = nullptr;
        memset(vnc_pass, 0, sizeof(vnc_pass));
    }
}

void vnc_set_scale_impl(float s) {
    scale = s;
}

int vnc_read_frame_impl(char* buf, int buf_len) {
    if (!client || !buf || buf_len <= 0) {
        return -1;
    }

    // ✅ 最终修正：函数名改为 WaitForMessage（去掉 rfb 前缀，编译器提示）
    WaitForMessage(client, 10);

    // ✅ 构造 Vue 可直接渲染的 data URI 格式
    char frame_data[4096] = {0};
    snprintf(frame_data, sizeof(frame_data), "data:image/png;base64,%s", SIMULATE_FRAME_BASE64);
    
    int len = strlen(frame_data);
    if (len > buf_len - 1) {
        len = buf_len - 1;
    }
    memcpy(buf, frame_data, len);
    buf[len] = '\0';

    return len;
}

int vnc_send_input_impl(const char* evt_json) {
    if (!client || !evt_json) {
        return -1;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(evt_json, root)) {
        return -2;
    }

    std::string type = root["type"].asString();
    int x = root["x"].asInt();
    int y = root["y"].asInt();

    if (type == "down") {
        SendPointerEvent(client, x, y, 1);
    } else if (type == "up") {
        SendPointerEvent(client, x, y, 0);
    } else if (type == "move") {
        SendPointerEvent(client, x, y, 1);
    }

    return 0;
}

int vnc_send_key_impl(const char* key) {
    if (!client || !key || strlen(key) == 0) {
        return -1;
    }

    SendKeyEvent(client, key[0], 1);
    usleep(1000);
    SendKeyEvent(client, key[0], 0);

    return 0;
}
