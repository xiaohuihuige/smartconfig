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
#include <stdint.h>
#include <stddef.h>  // C 标准库
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <stdatomic.h>

#include "log.h"

#define INVALID_SOCKET		        -1
#define MAC_LEN						6
#define IPADDR_LEN					4
#define PACKET_MAX					1500

typedef int SOCKET;
typedef unsigned char BOOL;

#define SMART_SUCCESS (0)
#define SMART_FAIL (-1)

#define GUIDECODE_DLY_MS			8
#define delay_ms(MS)	usleep(1000 * MS)

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

// 定义互斥锁结构体
typedef struct {
    pthread_cond_t cond;
} Cond;

// 初始化条件信号
#define COND_INIT(m) do { \
    if (pthread_cond_init(&(m).cond, NULL) != 0) { \
        ERR("Cond initialization failed"); \
    } \
} while (0)

// 销毁条件信号
#define COND_DESTROY(m) do { \
    if (pthread_cond_destroy(&(m).cond) != 0) { \
        ERR("Cond destruction failed"); \
    } \
} while (0)

// 发送信号
#define COND_SIGNAL(m) do { \
    if (pthread_cond_signal(&(m).cond) != 0) { \
        ERR("Cond signal failed"); \
    } \
} while (0)

// 等待阻塞条件
#define COND_WAIT(m, l) do { \
    if (pthread_cond_wait(&(m).cond, &(l).mutex) != 0) { \
        ERR("Cond signal failed"); \
    } \
} while (0)

// 发送全部信号
#define COND_BROADCAST(m) do { \
    if (pthread_cond_broadcast((&(m).cond)) != 0) { \
        ERR("Cond broadcast failed"); \
    } \
} while (0)

// 定义互斥锁结构体
typedef struct {
    pthread_mutex_t mutex;
} Mutex;

// 初始化互斥锁
#define MUTEX_INIT(m) do { \
    if (pthread_mutex_init(&((m).mutex), NULL) != 0) { \
        ERR("Mutex initialization failed"); \
    } \
} while (0)

// 销毁互斥锁
#define MUTEX_DESTROY(m) do { \
    if (pthread_mutex_destroy(&((m).mutex)) != 0) { \
        ERR("Mutex destruction failed"); \
    } \
} while (0)

// 加锁
#define MUTEX_LOCK(m) do { \
    if (pthread_mutex_lock(&((m).mutex)) != 0) { \
        ERR("Mutex lock failed"); \
    } \
} while (0)

// 解锁
#define MUTEX_UNLOCK(m) do { \
    if (pthread_mutex_unlock(&((m).mutex)) != 0) { \
        ERR("Mutex unlock failed"); \
    } \
} while (0)


#endif