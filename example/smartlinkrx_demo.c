#include <stdio.h>
#include "common.h"
#include "smartcfg.h"
#include <stdbool.h>

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

static int smartLinkRxFunc(void *cbctx, uint8_t *mac, int mac_len, uint8_t *buf, int len)
{
    LOG("data: %s", buf);
    return SMART_SUCCESS;
}

int main()
{
    captureErrorInit(signal_handler);

    void* handle = smartLinkRxInit("wlan0", 38, smartLinkRxFunc, NULL);
    if (!handle) 
        return EXIT_FAILURE;

    while (!gThreadExit) 
        sleep(1);

    smartLinkRxDeInit(handle);
    return EXIT_SUCCESS;
}