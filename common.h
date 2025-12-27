#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <stdarg.h>
#include <stdint.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>		/* for ETH_P_ALL */

#include "log.h"

#define INVALID_SOCKET		        -1
#define MAC_LEN						6
#define IPADDR_LEN					4
#define PACKET_MAX					1500

typedef int SOCKET;
typedef unsigned char BOOL;

#define SMART_SUCCESS (0)
#define SMART_FAIL (-1)

#define CLOSE_SOCKET(sockfd)    \
    do {                        \
        if (sockfd > 0) {        \
            close(sockfd);       \
            sockfd = INVALID_SOCKET;    \
        }                        \
    } while (0)

#define SMART_FREE(ptr)               \
    do {                        \
        if (ptr) {              \
            free(ptr);          \
            ptr = NULL;         \
        }                       \
    } while (0) 

#ifndef STCHECKRESULT
#define STCHECKRESULT(_func_)                                                              \
    do                                                                                     \
    {                                                                                      \
        int32_t s32Ret = SMART_SUCCESS;                                                        \
        s32Ret        = _func_;                                                            \
        if (s32Ret != SMART_SUCCESS)                                                          \
        {                                                                                  \
            ERR("exec function failed, error:%x", s32Ret);                                 \
            return s32Ret;                                                                 \
        }                                                                                  \
        else                                                                               \
        {                                                                                  \
            LOG("exec function pass");                                                     \
        }                                                                                  \
    } while (0)
#endif

#ifndef STCHECKRESULTS
#define STCHECKRESULTS(_func_)                                                              \
    do                                                                                     \
    {                                                                                      \
        int32_t s32Ret = SMART_SUCCESS;                                                        \
        s32Ret        = _func_;                                                            \
        if (s32Ret != SMART_SUCCESS)                                                          \
        {                                                                                  \
            ERR("exec function failed, error:%x, %s", s32Ret, strerror(errno));                                 \
            return s32Ret;                                                                 \
        }                                                                                  \
    } while (0)
#endif

#define ST_CHECK_POINTER(pointer)        \
    {                                    \
        if (pointer == NULL)             \
        {                                \
            ERR("NULL pointer!!!"); \
            return SMART_FAIL;          \
        }                                \
    }

#define ST_CHECK_POINTER_NULL(pointer)   \
    {                                    \
        if (pointer == NULL)             \
        {                                \
            ERR("NULL pointer!!!"); \
            return NULL;                   \
        }                                \
    }

#define ST_CHECK_POINTER_VOID(pointer)   \
    {                                    \
        if (pointer == NULL)             \
        {                                \
            ERR("NULL pointer!!!");   \
            return;                      \
        }                                \
    }

#define ST_CHECK_SOCKET(sockfd)          \
    {                                    \
        if (sockfd <= 0)                 \
        {                                \
            ERR("sockfd error!!!");      \
            return SMART_FAIL;           \
        }                                \
    }

#define ST_CHECK_SOCKET_NULL(sockfd)     \
    {                                    \
        if (sockfd <= 0)                 \
        {                                \
            ERR("sockfd error!!!");      \
            return NULL;           \
        }                                \
    }

#endif