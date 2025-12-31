#include "smartcfg.h"
#include "sockets.h"
#include "common.h"

#define START_FLAGS  (0x7F)
#define END_FLAGS    (0x7F)

typedef struct 
{
    uint8_t u32Index;
    uint8_t u32Sequence;
    SOCKET sockfd;
    Mutex mutex;
} SmartLinkTx_t;

static int _smartlinkTxSockConfig(SOCKET sockfd, const char *dev)
{
    ST_CHECK_SOCKET(sockfd);
    ST_CHECK_POINTER(dev);
    STCHECKRESULT(socketSetBroadcast(sockfd));
    STCHECKRESULT(socketBindIfcdev(sockfd, dev));
    return SMART_SUCCESS;
}

static int _smartlinkTxSendto(SOCKET sockfd, int sendlen)
{
    ST_CHECK_SOCKET(sockfd);
    uint8_t packet[PACKET_MAX] = {0};
    int n = socketSendtoBroadcast(sockfd, packet, sendlen);
    if (n < 0) {
        ERR("%s: %s (%d)", "sendto failed", strerror(errno), errno);
        return SMART_FAIL;
    }
    return SMART_SUCCESS;
}

void *smartlinktTxInit(const char *dev)
{
    ST_CHECK_POINTER_NULL(dev);

    SmartLinkTx_t *tx = (SmartLinkTx_t *)calloc(1, sizeof(SmartLinkTx_t));
    if (!tx) {
        ERR("malloc error");
        return NULL;
    }

    MUTEX_INIT(tx->mutex);

    do {
        tx->sockfd = createUdpSocket();
        if (tx->sockfd == INVALID_SOCKET) {
            ERR("createUdpSocket error");
            break;
        }

        if (_smartlinkTxSockConfig(tx->sockfd, dev) != SMART_SUCCESS) {
            ERR("smartlinkTxSockConfig error");
            break;
        }
        LOG("smartlinktx_init success");
        return (void *)tx;
    } while (0);

    CLOSE_SOCKET(tx->sockfd);

    MUTEX_DESTROY(tx->mutex);

    SMART_FREE(tx);

    return NULL;
}

void smartlinkTxDeInt(void *ctx)
{
    ST_CHECK_POINTER_VOID(ctx);
    SmartLinkTx_t *tx = (SmartLinkTx_t *)ctx;
    MUTEX_LOCK(tx->mutex);
    CLOSE_SOCKET(tx->sockfd);
    MUTEX_UNLOCK(tx->mutex);
    SMART_FREE(tx);
}

// int sendlen0 = ((L(n-1) ^ seq++) << 4) | H(n);
// int sendlen1 = ((H(n) ^ (seq++)) << 4) | L(n);

void smartlinkTxSend(void *ctx, uint8_t *input, uint8_t size, int interval_ms)
{
    ST_CHECK_POINTER_VOID(ctx);
    ST_CHECK_POINTER_VOID(input);

    SmartLinkTx_t *tx = (SmartLinkTx_t *)ctx;

    uint8_t length = size+2;

    uint8_t *buffer =(uint8_t *)malloc(sizeof(length));
    if (!buffer) {
        ERR("malloc error");
        return;
    }

    buffer[0] = START_FLAGS;
    memcpy(buffer+1, input, size);
    buffer[size+1] = END_FLAGS;

    MUTEX_LOCK(tx->mutex);
    tx->u32Sequence = 0x00;
    for (uint8_t u8Index = 0; u8Index < length; u8Index++) {
        uint8_t high1  = buffer[u8Index] >> 4;
        uint8_t low1   = buffer[u8Index] & 0xF;

        uint8_t low0  = u8Index != 0 ? ((buffer[u8Index-1] & 0xF) ^ tx->u32Sequence++) : (tx->u32Sequence++);
        uint8_t highs = high1 ^ (tx->u32Sequence++);

        int sendlen0 = ((low0 << 4) | high1) + 24;
        int sendlen1 = ((highs << 4) | low1)+ 24;

        _smartlinkTxSendto(tx->sockfd,  sendlen0);
        _smartlinkTxSendto(tx->sockfd,  sendlen1);
        LOG("u8Index %d, sendlen0: %d, sendlen1: %d", u8Index, sendlen0, sendlen1);
        delay_ms(interval_ms);
    }

    MUTEX_UNLOCK(tx->mutex);
}
