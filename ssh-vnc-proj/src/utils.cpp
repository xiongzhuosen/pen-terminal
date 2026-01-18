#include "ssh_vnc_full.h"
#include <stdlib.h>
#include <string.h>

// 工具函数：字符串分割、内存释放等
void utils_free(void* ptr) {
    if(ptr) free(ptr);
}
