#include "sockets.h"

SOCKET create_udp_socket(void)
{
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
		ERR("%s: %s (%d)", "socket() failed", strerror(errno), errno);
		return INVALID_SOCKET;
	}
	return sockfd;   
}

SOCKET create_raw_socket(void)
{
	SOCKET sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd == INVALID_SOCKET) {
		ERR("%s: %s (%d)", "socket() failed", strerror(errno), errno);
		return INVALID_SOCKET;
	}
	return sockfd; 
}

int socket_set_broadcast(SOCKET sockfd)
{
    ST_CHECK_SOCKET(sockfd);
    int broadcast_enable = 1;
    STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)));
    return SMART_SUCCESS;
}

int socket_set_noblocking(SOCKET sockfd)
{
    ST_CHECK_SOCKET(sockfd);
    STCHECKRESULTS(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0);
    return SMART_SUCCESS;
}

int socket_set_revcbuffer(SOCKET sockfd, int size)
{
    ST_CHECK_SOCKET(sockfd);
    int opt = size;
	STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&opt, sizeof(opt)));
    return SMART_SUCCESS;
}

int socket_revc_rawpacket(SOCKET sockfd)
{
    ST_CHECK_SOCKET(sockfd);
	uint8_t buffer[PACKET_MAX] = {0};
 	int rxlen = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, NULL, NULL);
	if (rxlen > 0) {
        parse_ieee80211_radiotap_header(buffer, rxlen);
	}
    return rxlen;
}

int socket_bind_ifcdev(SOCKET sockfd, const char *dev_name)
{
    ST_CHECK_SOCKET(sockfd);
    ST_CHECK_POINTER(dev_name);
    struct ifreq ifr = {0};
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", dev_name);
	STCHECKRESULTS(setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr)));  
    return SMART_SUCCESS; 
}



// void socket_bind_setup_add(struct sockaddr_in* addr, const char* addrstr, uint16_t port)
// {
// 	memset(addr, 0, sizeof(struct sockaddr_in));
// 	addr->sin_family = AF_INET;
// 	addr->sin_port	= htons(port);
// 	addr->sin_addr.s_addr = inet_addr(addrstr); // INADDR_ANY / INADDR_BROADCAST
// }

// void socket_send_to(SOCKET sockfd, uint32_t size, uint16_t port, const char *dstaddr)
// {
// 	char packet[PACKET_MAX];
// 	struct sockaddr_in addr = {0};
// 	memset(&addr, 0, sizeof(struct sockaddr_in));
// 	addr.sin_family = AF_INET;
// 	addr.sin_port	= htons(port);
// 	addr.sin_addr.s_addr = inet_addr(dstaddr); 
// 	int n = sendto(sockfd, packet, size, 0, (struct sockaddr*)(&addr), sizeof(struct sockaddr_in));
// 	if (n < 0)
// 		printf("%s: %s (%d)\n", "sendto failed", strerror(errno), errno);
// }

// int socket_bind_addr(SOCKET sockfd, uint16_t port, const char *dstaddr)
// {
//     struct sockaddr_in addr = {0};
//     addr.sin_family = AF_INET;		  
// 	addr.sin_addr.s_addr = inet_addr(dstaddr);
// 	addr.sin_port = htons(port);
    
//     if (bind(sockfd,  (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//         printf("%s: %s (%d)\n", "bind failed", strerror(errno), errno);
//         CLOSE_SOCKET(sockfd);
//         return SMART_FAIL;
//     }
//     return SMART_SUCCESS;
// }

