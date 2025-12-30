#ifndef __SMARTCFG_H__
#define __SMARTCFG_H__

#include "common.h"

typedef enum {
    SMARTLINK_DATLEN_MODE,
    SMARTLINK_IPADDR_MODE,
} SmartLinkMode_e;

void* smartlinktx_init(const char *dev);
void  smartlinktx_exit(void *ctx);
void  smartlinktx_send(void *ctx, uint8_t *buf, int len, int interval_ms);

typedef int (*SmartLinkRxFunc)(void *cbctx, uint8_t *mac, uint8_t *buf, int len);
void* smartLinkRxInit(const char *dev, uint16_t u16Channel, SmartLinkRxFunc smartLinkRx, void *cbctx);
void smartLinkRxDeInit(void *ctx);

#endif