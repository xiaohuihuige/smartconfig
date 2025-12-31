#include "sockets.h"

SOCKET createUdpSocket(void)
{
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
		ERR("%s: %s (%d)", "socket() failed", strerror(errno), errno);
		return INVALID_SOCKET;
	}
	return sockfd;   
}

SOCKET createRawSocket(void)
{
	SOCKET sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd == INVALID_SOCKET) {
		ERR("%s: %s (%d)", "socket() failed", strerror(errno), errno);
		return INVALID_SOCKET;
	}
	return sockfd; 
}

int socketSetBroadcast(SOCKET sockfd)
{
    ST_CHECK_SOCKET(sockfd);
    int broadcast_enable = 1;
    STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)));
    return SMART_SUCCESS;
}

int socketSetNoblocking(SOCKET sockfd)
{
    ST_CHECK_SOCKET(sockfd);
    STCHECKRESULTS(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0);
    return SMART_SUCCESS;
}

int socketSetRevcbuffer(SOCKET sockfd, int size)
{
    ST_CHECK_SOCKET(sockfd);
    int opt = size;
	STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&opt, sizeof(opt)));
    return SMART_SUCCESS;
}

int socketRevcRawpacket(SOCKET sockfd, ieee80211_radiotap *radiotap)
{
    ST_CHECK_SOCKET(sockfd);
	uint8_t buffer[PACKET_MAX] = {0};
 	int rxlen = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, NULL, NULL);
	if (rxlen > 0) {
        return ieee80211RadiotapHeader(radiotap, buffer, rxlen);
	}
    return rxlen;
}

int socketBindIfcdev(SOCKET sockfd, const char *dev_name)
{
    ST_CHECK_SOCKET(sockfd);
    ST_CHECK_POINTER(dev_name);
    struct ifreq ifr = {0};
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", dev_name);
	STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr)));  
    return SMART_SUCCESS; 
}


int socketSendtoBroadcast(SOCKET sockfd, uint8_t *buffer, int size)
{
    ST_CHECK_SOCKET(sockfd);
    ST_CHECK_POINTER(buffer);

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
	addr.sin_port        = 1 + rand() % 0XFFFE;
	addr.sin_addr.s_addr = 0XFFFFFFFF; // INADDR_ANY / INADDR_BROADCAST
    return sendto(sockfd,buffer, size, 0, (struct sockaddr*)(&addr), (socklen_t)sizeof(addr));; 
}


