#include "smartcfg.h"
#include "sockets.h"
#include "common.h"

typedef struct 
{
    SOCKET sockfd;
    volatile sig_atomic_t threadExit;
    pthread_t thread;
} SmartLinktx_t;

static SmartLinktx_t *_smartlinktx_init(const char *dev)
{
    SmartLinktx_t *tx = (SmartLinktx_t *)calloc(1, sizeof(SmartLinktx_t));
    if (!tx) {
        ERR("malloc error");
        return NULL;
    }

    do  {
        tx->sockfd = createUdpSocket();
        if (tx->sockfd == INVALID_SOCKET) {
            ERR("createUdpSocket error");
            break;
        }

        if (socketSetBroadcast(tx->sockfd) != SMART_SUCCESS) {
            ERR("socketSetBroadcast error");
            break;
        }

        if (socketBindIfcdev(tx->sockfd, dev)) {
            ERR("socketBindIfcdev error");
            break;
        }
        LOG("smartlinktx_init success");
        return (void *)tx;
    } while (0);
   
    CLOSE_SOCKET(tx->sockfd);
    SMART_FREE(tx);

    return NULL;
}

static void _smartlinktx_exit(void *ctx)
{
    ST_CHECK_POINTER_VOID(ctx);
    CLOSE_SOCKET(((SmartLinktx_t *)ctx)->sockfd);
    SMART_FREE(ctx);
}

static int smartlinktx_sendto(SmartLinktx_t *tx, int sendlen)
{
    uint8_t packet[PACKET_MAX] = {0};
    int n = socketSendtoBroadcast(tx->sockfd, packet, sendlen);
    if (n < 0) {
        ERR("%s: %s (%d)", "sendto failed", strerror(errno), errno);
        return SMART_FAIL;
    }
    return SMART_SUCCESS;
}

static void _smartlinktx_send(SmartLinktx_t *tx, uint8_t *buffer, uint8_t size, int interval_ms)
{
    ST_CHECK_POINTER_VOID(tx);
    ST_CHECK_POINTER_VOID(buffer);

    uint8_t seq = 0x00;
    for (uint8_t idx = 0; idx < size; idx++) {
        uint8_t high  = (buffer[idx] >> 4) & 0x0F;
        uint8_t low   = buffer[idx] & 0x0F;

        uint8_t A0 = idx!= 0 ? ((buffer[idx-1] & 0x0F) ^ seq++) :(seq++);
        uint8_t B0 = high ^ (seq++);

        int sendlen0 = ((A0 << 4) | high) + 24;
        int sendlen1 = ((B0 << 4) | low)+ 24;

        LOG("%03X, %03X, idx :%d, hight %03X, low %03X", A0, B0, idx , high, low);
        smartlinktx_sendto(tx,  sendlen0);
        smartlinktx_sendto(tx,  sendlen1);
        LOG("seq %d, sendlen0: %d, sendlen1: %d", seq, sendlen0, sendlen1);
        delay_ms(interval_ms);
    }
}

void smartlinktx_send(void *ctx, uint8_t *buf, int len, int interval_ms)
{
    ST_CHECK_POINTER_VOID(ctx);
    ST_CHECK_POINTER_VOID(buf);
    return _smartlinktx_send((SmartLinktx_t *)ctx, buf, len, interval_ms);
}

void* smartlinktx_init(const char *dev)
{
    ST_CHECK_POINTER_NULL(dev);
    return (void *)_smartlinktx_init(dev);
}

void smartlinktx_exit(void *ctx)
{
    ST_CHECK_POINTER_VOID(ctx);
    return _smartlinktx_exit(ctx);
}
