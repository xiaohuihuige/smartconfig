#ifndef __IFACE_H__
#define __IFACE_H__

#include "common.h"

int ifaceSeMonitorMode(SOCKET sock_fd, const char *device);
int ifaceSetFreq(SOCKET sock_fd, const char *device, uint16_t u16Channel);
int ifaceBindSocket(SOCKET sock_fd, const char *device, int32_t s32Protocol);

#endif