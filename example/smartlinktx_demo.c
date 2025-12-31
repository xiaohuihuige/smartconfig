#include <stdio.h>
#include "common.h"
#include "smartcfg.h"

volatile sig_atomic_t gThreadExit = false;

void signal_handler(int signum) 
{
    switch (signum) {
        case SIGINT:
            ERR("Caught SIGINT (Ctrl+C)");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        case SIGTERM:
            ERR("Caught SIGTERM");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        case SIGQUIT:
            ERR("Caught SIGQUIT (Ctrl+\\)");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        case SIGHUP:
            ERR("Caught SIGHUP");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        case SIGSEGV:
            ERR("Caught SIGSEGV");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        case SIGPIPE:
            ERR("Caught SIGPIPE");
            gThreadExit = true; // 设置标志以指示程序应退出
            break;
        default:
            break;
    }
    //exit(EXIT_FAILURE);
}

void captureErrorInit(void (*signal_handler)(int))
{
    if (!signal_handler) {
        ERR("algoCaptureErrorInit args error");
        return;
    }
    
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGSEGV, SIGPIPE};
    size_t num_signals = sizeof(signals) / sizeof(signals[0]);

    for (size_t i = 0; i < num_signals; i++) {
        if (sigaction(signals[i], &sa, NULL) == -1) {
            ERR("sigaction error");
            exit(EXIT_FAILURE);
        }
    }
}


int main(int argc, char *argv[])
{
    captureErrorInit(signal_handler);

    if (argc != 2) {
        printf("用法: %s <参数>\n", argv[0]);
        return 1; // 返回错误代码
    }

    // 获取传入的参数
    char *input = argv[1];
    if (!input)
        return SMART_FAIL;

    void *handle = smartlinktTxInit("wlan0");
    if (!handle)
        return SMART_FAIL;

    while (!gThreadExit) 
        smartlinkTxSend(handle, (uint8_t *)input , strlen(input), 100);

    smartlinkTxDeInt(handle);
}