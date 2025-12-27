#include <stdio.h>
#include "common.h"
#include "sockets.h"
#include "smartcfg_client.h"

int main()
{
    SmartcfgClient_t *smcli = smartcfgClientInit();
    if (!smcli)
        return -1;
    while (1) {
        socket_revc_rawpacket(smcli->sockfd);
    }
    smartcfgClientDeInit(smcli);
    return 0;
}