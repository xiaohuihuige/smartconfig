#include "smartcfg.h"
#include "sockets.h"
#include "iface.h"
#include "common.h"
#include "smartcfg.h"

#define BUFFER_MAX   (512)
#define START_FLAGS  (0x7F)
#define END_FLAGS    (0x7F)

typedef struct 
{
    void *userData;
    SmartLinkRxFunc smartLinkRx;
    SOCKET sockFd;
    uint8_t u32Index;
    uint8_t u32Sequence;
    uint8_t sourceAddr[MAC_LEN];
    uint8_t buffer[BUFFER_MAX];
    uint16_t u16Channel;
    pthread_t thread;
    volatile sig_atomic_t threadExit;
} SmartLinkRx_t;

static int _smartLinkRxSockConfig(SOCKET sockFd, const char *dev, uint16_t u16Channel)
{
    ST_CHECK_POINTER(dev);
    ST_CHECK_SOCKET(sockFd);
    STCHECKRESULT(socketBindIfcdev(sockFd, dev));
    STCHECKRESULT(ifaceSeMonitorMode(sockFd, dev));
    STCHECKRESULT(ifaceSetFreq(sockFd, dev, u16Channel));
    STCHECKRESULT(socketSetNoblocking(sockFd));
    STCHECKRESULT(socketSetRevcbuffer(sockFd, 25 * 1024));
    return SMART_SUCCESS;
}

static void _smartlinkRxDecode(SmartLinkRx_t *ctx, int sendlen, ieee80211_radiotap *radiotap)
{
    ST_CHECK_POINTER_VOID(ctx);

    uint8_t data  = (sendlen - 24) >> 0 & 0xF; //低
    uint8_t flag  = (sendlen - 24) >> 4;       //高

    if (ctx->u32Sequence % 2 == 0) {
        uint8_t pdata = ctx->u32Index == 0x00 ? 0x00 : ctx->buffer[ctx->u32Index-1] & 0xF;
        if (((pdata ^ ctx->u32Sequence) != flag)) 
            return;

        ctx->buffer[ctx->u32Index] |= data << 4; //高位赋值
    } else {
        uint8_t pdata = ctx->buffer[ctx->u32Index] >> 4;
        if (((pdata ^ ctx->u32Sequence) != flag)) {
            if (ctx->u32Index == 0x00 && data != 0x7) 
                ctx->u32Sequence = 0x00;
            return;
        }

        ctx->buffer[ctx->u32Index++] |= data;//低位赋值

        if (ctx->u32Index > 0x02) 
            memcpy(ctx->sourceAddr, radiotap->saddr, sizeof(radiotap->saddr));

        LOG("revc the data: %03X", ctx->buffer[ctx->u32Index-1]);

        if (ctx->u32Index > 0x02 && ctx->buffer[ctx->u32Index-1] == END_FLAGS) {
            ctx->buffer[ctx->u32Index-1] = '\0';
            ctx->smartLinkRx(ctx->userData, ctx->sourceAddr, MAC_LEN, ctx->buffer + 1, ctx->u32Index);
        }
    }
    ctx->u32Sequence++;
}

static void * _smartlinkRxProcess(void *args)
{
    ST_CHECK_POINTER_NULL(args);
    SmartLinkRx_t *ctx= (SmartLinkRx_t *)args;

    ieee80211_radiotap radiotap = {0};

    while (!ctx->threadExit) {
        int sendlen = socketRevcRawpacket(ctx->sockFd, &radiotap);
        if (sendlen <= 0)
            continue;
        _smartlinkRxDecode(ctx, sendlen, &radiotap);
    }
    return NULL;
}

void smartLinkRxDeInit(void *ctx)
{
    ST_CHECK_POINTER_VOID(ctx);

    SmartLinkRx_t *receivers = (SmartLinkRx_t *)ctx;

    if (!receivers->threadExit) {
        receivers->threadExit  = true;
        if (pthread_join(receivers->thread , NULL) != 0) {
            ERR("Failed to join thread");
            return;
        }
    }
    CLOSE_SOCKET(receivers->sockFd);

    SMART_FREE(receivers);
}

void* smartLinkRxInit(const char *dev, uint16_t u16Channel, SmartLinkRxFunc smartLinkRx, void *cbctx)
{
    ST_CHECK_POINTER_NULL(dev);

    SmartLinkRx_t *receivers = (SmartLinkRx_t *)calloc(1, sizeof(SmartLinkRx_t));
    if (!receivers) {
        ERR("malloc error");
        return NULL;
    }

    do {
        receivers->sockFd = createRawSocket();
        if (receivers->sockFd <= 0) {
            ERR("socket fail");
            break;
        }

        if (SMART_SUCCESS != _smartLinkRxSockConfig(receivers->sockFd, dev, u16Channel)) {
            ERR("_smartLinkRxSockConfig fail");
            break;
        }

        memset(receivers->buffer, 0x00, sizeof(receivers->buffer));
        receivers->u32Index    = 0x00;
        receivers->u32Sequence = 0x00;
        receivers->u16Channel  = u16Channel;
        receivers->smartLinkRx = smartLinkRx;
        receivers->userData    = cbctx;
        receivers->threadExit  = false;

        int ret = pthread_create(&receivers->thread, NULL, _smartlinkRxProcess, (void *)receivers); 
        if (ret != SMART_SUCCESS) {
            ERR("Failed to create thread");
            break;
        }
        return (void*)receivers;
    } while (0);

    smartLinkRxDeInit(receivers);

    return NULL;
}


