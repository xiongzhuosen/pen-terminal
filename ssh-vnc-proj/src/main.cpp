#include "ssh_vnc_full.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
    // 初始化所有模块
    ssh_init("");
    vnc_init("");
    keyboard_init();

    // 等待用户操作
    while(1) {
        sleep(1);
    }

    // 清理资源
    ssh_close();
    vnc_close();
    keyboard_close();
    return 0;
}
