#include <stdio.h>
#include "common.h"
#include "smartcfg.h"

static int smartLinkRxFunc(void *cbctx, uint8_t *mac, uint8_t *buf, int len)
{
    LOG("data: %s", buf);

    return SMART_SUCCESS;
}

int main()
{
    void* handle = smartLinkRxInit("wlan0", 38, smartLinkRxFunc, NULL);
    if (!handle) 
        return EXIT_FAILURE;

    while (1) 
        sleep(1);
    smartLinkRxDeInit(handle);
    return EXIT_SUCCESS;
}