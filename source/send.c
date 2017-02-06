#include <errno.h>
#include <sys/socket.h>

#include "encoder.h"
#include "packet.h"
#include "send.h"

int send_packet(int sockfd, void *packet, size_t packet_size) {
	int res = send(sockfd, packet, packet_size, 0);

	return res;
}

int send_both(int sockfd, void *data, size_t body_size) {
	size_t packet_size;

	void *packet = make_both(&packet_size, data, body_size);

	int res = send_packet(sockfd, packet, packet_size);

	free(packet);
	packet = NULL;

	return res;
}

int send_bencode(int sockfd, Bencode *obj) {
	printf("send: %s\n", ben_str_val(ben_list_get(obj, 0)));

	size_t data_size;

	void *data = ben_encode(&data_size, obj);

	int res = send_both(sockfd, data, data_size);

	free(data);
	data = NULL;

	return res;
}
