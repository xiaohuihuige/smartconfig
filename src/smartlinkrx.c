#include "smartcfg.h"
#include "sockets.h"
#include "iface.h"
#include "common.h"
#include "smartcfg.h"
#include "list.h"

#define BUFFER_MAX (512)

typedef struct 
{
    void *userData;
    SmartLinkRxFunc smartLinkRx;
    SOCKET sockFd;
    uint8_t u32Index;
    uint8_t u32Sequence;
    uint8_t buffer[BUFFER_MAX];
    uint16_t u16Channel;
    Mutex mutex;
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

static void _smartlinkRxEncode(SmartLinkRx_t *ctx, int sendlen)
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
            if (ctx->u32Index == 0x00 && data != 0x7) {
                ctx->u32Sequence = 0x00;
                return;
            }
            return;
        }

        ctx->buffer[ctx->u32Index++] |= data;//低位赋值
        LOG("ctx->buffer %s",  ctx->buffer);
        if (ctx->u32Index != 0x01 && ctx->buffer[ctx->u32Index-1] == 0x7F) {
            ctx->buffer[ctx->u32Index-1] = '\0';
            uint8_t tem_buffer[100] = {0};
            snprintf((char *)tem_buffer, sizeof(tem_buffer), "%s", ctx->buffer + 1);
            ctx->smartLinkRx(ctx->userData, NULL, tem_buffer, ctx->u32Index -2);
        }
    }
    ctx->u32Sequence++;
}

static void * _smartlinkRxProcess(void *args)
{
    ST_CHECK_POINTER_NULL(args);
    SmartLinkRx_t *ctx= (SmartLinkRx_t *)args;

    while (!ctx->threadExit) {
        int sendlen = socketRevcRawpacket(ctx->sockFd);
        if (sendlen <= 0)
            continue;
        _smartlinkRxEncode(ctx, sendlen);
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

    MUTEX_DESTROY(receivers->mutex);

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

    MUTEX_INIT(receivers->mutex);

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


