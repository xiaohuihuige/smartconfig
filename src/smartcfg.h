#ifndef __SMARTCFG_H__
#define __SMARTCFG_H__

#include "common.h"

void *smartlinktTxInit(const char *dev);
void smartlinkTxDeInt(void *ctx);
void smartlinkTxSend(void *ctx, uint8_t *buffer, uint8_t size, int interval_ms);

typedef int (*SmartLinkRxFunc)(void *cbctx, uint8_t *mac, int mac_len, uint8_t *buf, int len);
void* smartLinkRxInit(const char *dev, uint16_t u16Channel, SmartLinkRxFunc smartLinkRx, void *cbctx);
void smartLinkRxDeInit(void *ctx);

#endif