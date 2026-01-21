#include "include/vnc_input.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static char vnc_pass[64] = {0};
static rfbClient* client = nullptr;
static float scale = 1.0;
static const char* SIMULATE_FRAME_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAAZdEVYdFNvZnR3YXJlAHBhaW50Lm5ldCA0LjAuNWWFMmUAAABoSURBVHhe7cExAQAAAMKg9U9tDQ+gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADwD5UAB6h9c4gAAAABJRU5ErkJggg==";

static int parse_vnc_event(const char* evt_json, char* type, int max_type_len, int* x, int* y) {
    if (!evt_json || !type || !x || !y) return -1;

    char* type_pos = strstr((char*)evt_json, "\"type\":\"");
    if (!type_pos) return -2;
    type_pos += 8;
    char* type_end = strchr(type_pos, '"');
    if (!type_end) return -3;
    int type_len = type_end - type_pos;
    if (type_len >= max_type_len) type_len = max_type_len - 1;
    memcpy(type, type_pos, type_len);
    type[type_len] = '\0';

    char* x_pos = strstr((char*)evt_json, "\"x\":");
    if (!x_pos) return -4;
    *x = atoi(x_pos + 4);

    char* y_pos = strstr((char*)evt_json, "\"y\":");
    if (!y_pos) return -5;
    *y = atoi(y_pos + 4);

    return 0;
}

static char* vnc_get_password(rfbClient* cl) {
    return vnc_pass;
}

int vnc_connect_impl(const char* ip, const char* port, const char* pass) {
    if (client) {
        vnc_disconnect_impl();
    }

    client = rfbGetClient(8, 3, 4);
    if (!client) return -1;

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
    if (!client || !buf || buf_len <= 0) return -1;

    WaitForMessage(client, 10);
    char frame_data[4096] = {0};
    snprintf(frame_data, sizeof(frame_data), "data:image/png;base64,%s", SIMULATE_FRAME_BASE64);

    int len = strlen(frame_data);
    if (len > buf_len - 1) len = buf_len - 1;
    memcpy(buf, frame_data, len);
    buf[len] = '\0';
    return len;
}

// ✅ 修正：vnc_send_input_impl → vnc_send_mouse_impl
int vnc_send_mouse_impl(const char* evt_json) {
    if (!client || !evt_json) return -1;

    char type[16] = {0};
    int x = 0, y = 0;
    int ret = parse_vnc_event(evt_json, type, sizeof(type), &x, &y);
    if (ret != 0) return ret;

    if (strcmp(type, "down") == 0) {
        SendPointerEvent(client, x, y, 1);
    } else if (strcmp(type, "up") == 0) {
        SendPointerEvent(client, x, y, 0);
    } else if (strcmp(type, "move") == 0) {
        SendPointerEvent(client, x, y, 1);
    }
    return 0;
}

int vnc_send_key_impl(const char* key) {
    if (!client || !key || strlen(key) == 0) return -1;
    SendKeyEvent(client, key[0], 1);
    usleep(1000);
    SendKeyEvent(client, key[0], 0);
    return 0;
}