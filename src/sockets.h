#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include "common.h"
#include "wlan80211.h"

SOCKET createUdpSocket(void);
SOCKET createRawSocket(void);

int socketSetBroadcast(SOCKET sockfd);
int socketSetNoblocking(SOCKET sockfd);
int socketBindIfcdev(SOCKET sockfd, const char *dev_name);
int socketRevcRawpacket(SOCKET sockfd);
int socketSetRevcbuffer(SOCKET sockfd, int size);
int socketSendtoBroadcast(SOCKET sockfd, uint8_t *buffer, int size);

#endif