#include <stdio.h>
#include "common.h"
#include "smartcfg.h"


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("用法: %s <参数>\n", argv[0]);
        return 1; // 返回错误代码
    }

    // 获取传入的参数
    char *input = argv[1];
    void* handle = smartlinktx_init("wlan0");
    if (!handle)
        return -1;


    char buffer[100] = {0};
    snprintf(buffer, sizeof(buffer), "%c%s%c", 0x7F, input, 0x7F);

    LOG("input %s, %d", buffer, strlen(buffer));
    while (1) {
        smartlinktx_send(handle, (uint8_t *)buffer, strlen(buffer), 100);
    }
    smartlinktx_exit(handle);
}