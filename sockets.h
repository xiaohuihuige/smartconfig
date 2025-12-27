#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include "common.h"
#include "wlan_80211.h"

SOCKET create_udp_socket(void);
SOCKET create_raw_socket(void);

int socket_set_broadcast(SOCKET sockfd);
int socket_set_noblocking(SOCKET sockfd);
int socket_bind_ifcdev(SOCKET sockfd, const char *dev_name);
int socket_revc_rawpacket(SOCKET sockfd);
int socket_set_revcbuffer(SOCKET sockfd, int size);

#endif