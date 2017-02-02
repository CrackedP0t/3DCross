#include <sys/socket.h>

#include "send.h"

int send_packet(int sockfd, void *packet, size_t packet_size) {
	int sendres = send(sockfd, packet, packet_size, 0);

	return sendres;
}
