#include "iface.h"

#define CHAN2G(_channel, _freq) {                  \
    .u16HwValue           = (_channel),           \
    .u16CenterFreq        = (_freq),              \
}

typedef struct  {
	uint16_t u16HwValue;
	uint16_t u16CenterFreq;
} Ieee80211Channel_t;

static Ieee80211Channel_t gchannels[] = 
{
	CHAN2G(1, 2412),
	CHAN2G(2, 2417),
	CHAN2G(3, 2422),
	CHAN2G(4, 2427),
	CHAN2G(5, 2432),
	CHAN2G(6, 2437),
	CHAN2G(7, 2442),
	CHAN2G(8, 2447),
	CHAN2G(9, 2452),
	CHAN2G(10, 2457),
	CHAN2G(11, 2462),
	CHAN2G(12, 2467),
	CHAN2G(13, 2472),
	CHAN2G(14, 2484),
	CHAN2G(34, 5170),
	CHAN2G(36, 5180),
	CHAN2G(38, 5190),
	CHAN2G(40, 5200),
	CHAN2G(42, 5210),
	CHAN2G(44, 5220),
};

static uint16_t _ifaceGetfreq(uint16_t clannel)
{
    for (uint32_t i = 0; 0 < sizeof(gchannels) / sizeof(gchannels[0]); i++) {
        if (gchannels[i].u16HwValue == clannel)
            return gchannels[i].u16CenterFreq;
    }
    return SMART_SUCCESS;
}

static int _ifaceGetFlags(SOCKET sock_fd, const char *device)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", device);
	STCHECKRESULTS(ioctl(sock_fd, SIOCGIFFLAGS, &ifr));
	return ifr.ifr_flags;
}

static BOOL _ifaceSetDown(SOCKET sock_fd, const char *device)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", device);

	STCHECKRESULTS(ioctl(sock_fd, SIOCGIFFLAGS, &ifr));

	if (ifr.ifr_flags & IFF_UP) {
		ifr.ifr_flags &= ~IFF_UP;
		STCHECKRESULTS(ioctl(sock_fd, SIOCSIFFLAGS, &ifr));
	}
	return SMART_SUCCESS;
}

static BOOL _ifaceSetUp(int sock_fd, const char *device, int oldflags)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));

    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", device);

	STCHECKRESULTS(ioctl(sock_fd, SIOCGIFFLAGS, &ifr));

	ifr.ifr_flags |= oldflags;
	ifr.ifr_flags |= IFF_UP;
	STCHECKRESULTS(ioctl(sock_fd, SIOCSIFFLAGS, &ifr));	
	return SMART_SUCCESS;
}

static int _ifaceSetMode(SOCKET sock_fd, const char *device, int mode)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	struct iwreq ireq;
	memset(&ireq, 0, sizeof(ireq));

    snprintf(ireq.ifr_ifrn.ifrn_name, sizeof(ireq.ifr_ifrn.ifrn_name), "%s", device);

	ireq.u.mode = mode;
	STCHECKRESULTS(ioctl(sock_fd, SIOCSIWMODE, &ireq));
	return SMART_SUCCESS;
}

int ifaceSeMonitorMode(SOCKET sock_fd, const char *device)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	int oldflags = _ifaceGetFlags(sock_fd, device);
	if (oldflags < 0) {
		ERR("Can't get %s flags", device);
		return SMART_FAIL;
	}

	STCHECKRESULTS(_ifaceSetDown(sock_fd, device));
	STCHECKRESULTS(_ifaceSetMode(sock_fd, device, IW_MODE_MONITOR));
	STCHECKRESULTS(_ifaceSetUp(sock_fd, device, oldflags));
	return SMART_SUCCESS;
}

int ifaceSetFreq(SOCKET sock_fd, const char *device, uint16_t u16Channel)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

    struct iwreq iwr;
	memset(&iwr, 0, sizeof(iwr));

    snprintf(iwr.ifr_name, sizeof(iwr.ifr_name), "%s", device);

	iwr.u.freq.m = _ifaceGetfreq(u16Channel) * 100000;
	iwr.u.freq.e = 1;
	iwr.u.freq.flags = IW_FREQ_FIXED; // 设置为固定频率

	STCHECKRESULTS(ioctl(sock_fd, SIOCSIWFREQ, &iwr) < 0);
	LOG("set channel success %d", iwr.u.freq.m);
	return SMART_SUCCESS;
}

int ifaceBindSocket(SOCKET sock_fd, const char *device, int32_t s32Protocol)
{
    ST_CHECK_POINTER(device);
    ST_CHECK_SOCKET(sock_fd);

	struct ifreq ifr;
	struct sockaddr_ll ll;
    memset(&ifr, 0, sizeof(ifr));
	memset(&ll, 0, sizeof(ll));

    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", device);

	ll.sll_family = PF_PACKET;
	ll.sll_ifindex = ifr.ifr_ifindex;
	ll.sll_protocol = s32Protocol;
    STCHECKRESULTS(ioctl(sock_fd, SIOCGIFINDEX, &ifr));
	STCHECKRESULTS(bind(sock_fd, (struct sockaddr *)&ll, sizeof(ll)));
    return SMART_SUCCESS;
}