#ifndef __SMARTCFG_H__
#define __SMARTCFG_H__

#include "common.h"

typedef struct 
{
    SOCKET sockfd;
} SmartcfgClient_t;

SmartcfgClient_t *smartcfgClientInit();
void smartcfgClientDeInit(SmartcfgClient_t *smcli);

#endif