#include "include/vnc_input.h"
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// ✅ 全局变量：存储VNC密码（供回调函数使用）
static char vnc_pass[64] = {0};
static rfbClient* client = nullptr;
static float scale = 1.0;
static char frame_buf[4096] = {0};

// ✅ VNC密码回调函数：libvncclient会调用此函数获取密码
static char* vnc_get_password(rfbClient* cl) {
    return vnc_pass;
}

// ✅ 函数名加_impl后缀，适配导出逻辑
int vnc_connect_impl(const char* ip, const char* port, const char* pass) {
    if (client) {
        vnc_disconnect_impl();
    }

    // 1. 初始化VNC客户端
    client = rfbGetClient(8, 3, 4);
    if (!client) {
        return -1; // 客户端创建失败
    }

    // 2. 设置服务器地址和端口
    client->serverHost = (char*)ip;
    client->serverPort = atoi(port);

    // 3. 设置密码回调函数（关键：替代直接赋值password成员）
    strncpy(vnc_pass, pass, sizeof(vnc_pass) - 1);
    client->GetPassword = vnc_get_password;

    // 4. 初始化VNC连接（去掉rfb前缀）
    if (!InitClient(client, nullptr, nullptr)) {
        rfbClientCleanup(client);
        client = nullptr;
        return -2; // 连接初始化失败
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

    // ✅ 去掉rfb前缀：ProcessEvents替代rfbProcessEvents
    ProcessEvents(client, 10);

    // 模拟帧数据（实际需从client->frameBuffer读取像素并编码）
    snprintf(frame_buf, sizeof(frame_buf), "data:image/png;base64,%s", "FRAME_DATA_HERE");
    int len = strlen(frame_buf);
    if (len > buf_len - 1) {
        len = buf_len - 1;
    }
    memcpy(buf, frame_buf, len);
    buf[len] = '\0';

    return len;
}

int vnc_send_input_impl(const char* evt_json) {
    if (!client || !evt_json) {
        return -1;
    }

    // 解析JSON事件
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(evt_json, root)) {
        return -2; // JSON解析失败
    }

    std::string type = root["type"].asString();
    int x = root["x"].asInt();
    int y = root["y"].asInt();

    // ✅ 去掉rfb前缀：SendPointerEvent替代rfbSendPointerEvent
    // ✅ rfbButton1Mask替换为数值1（对应鼠标左键）
    if (type == "down") {
        SendPointerEvent(client, x, y, 1, 1);
    } else if (type == "up") {
        SendPointerEvent(client, x, y, 0, 0);
    } else if (type == "move") {
        SendPointerEvent(client, x, y, 1, 1);
    }

    return 0;
}

int vnc_send_key_impl(const char* key) {
    if (!client || !key || strlen(key) == 0) {
        return -1;
    }

    // ✅ 去掉rfb前缀：SendKeyEvent替代rfbSendKeyEvent
    SendKeyEvent(client, key[0], 1);
    usleep(1000);
    SendKeyEvent(client, key[0], 0);

    return 0;
}
