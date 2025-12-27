#include "smartcfg_client.h"
#include "sockets.h"
#include "iface.h"

SmartcfgClient_t *smartcfgClientInit()
{   
    SmartcfgClient_t *smcli = (SmartcfgClient_t *)malloc(sizeof(SmartcfgClient_t));
    if (!smcli) {
        printf("malloc error\n");
        return NULL;
    }

    do {
        smcli->sockfd = create_raw_socket();
        if (smcli->sockfd <= 0) {
            ERR("socket fail");
            break;
        }
    
        if (SMART_SUCCESS != socket_bind_ifcdev(smcli->sockfd, "wlan0")) {
            ERR("socket_bind_ifcdev fail");
            break;
        }
        
        if (SMART_SUCCESS != ifaceSeMonitorMode(smcli->sockfd, "wlan0")) {
            ERR("ifaceSeMonitorMode fail");
            break;
        }

        if (SMART_SUCCESS != ifaceSetFreq(smcli->sockfd, "wlan0", 38)) {
            ERR("ifaceSetFreq fail");
            break;
        }

        if (SMART_SUCCESS != socket_set_noblocking(smcli->sockfd)) {
            ERR("socket_set_noblocking fail");
            break;
        }

        if (SMART_SUCCESS != socket_set_revcbuffer(smcli->sockfd, 16 * 1024)) {
            ERR("socket_set_revcbuffer fail");
            break;
        }

        LOG("creatr smartconfig success");
        return smcli;
    } while(0);

    CLOSE_SOCKET(smcli->sockfd);

    SMART_FREE(smcli);

    return NULL;
}

void smartcfgClientDeInit(SmartcfgClient_t *smcli)
{
    CLOSE_SOCKET(smcli->sockfd);
    SMART_FREE(smcli);
}

// SOCKET sockfd = create_udp_socket();
//     if (sockfd == INVALID_SOCKET) {
//         printf("error");
//         return -1;
//     }

//     //socket_set_blocking(sockfd, true);
//     socket_bind_network(sockfd, "wlan0");

//     socket_set_broadcast(sockfd);

//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
// 	addr.sin_port	= htons(7001);
// 	addr.sin_addr.s_addr = 0XFFFFFFFF; // INADDR_ANY / INADDR_BROADCAST

//     while (1) {
//         char packet[PACKET_MAX];
//         int size = 1024;
//         addr.sin_port = 1 + rand() % 0XFFFE;
//         int n = sendto(sockfd, packet, size, 0, (struct sockaddr*)(&addr), (socklen_t)sizeof(addr));
//         if (n < 0)
//             printf("%s: %s (%d)\n", "sendto failed", strerror(errno), errno);
//         delay_ms(200);
//         printf("size: %d \n",n);
//     }