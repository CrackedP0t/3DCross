#include <sys/socket.h>

#include "encoder.h"
#include "packet.h"
#include "recieve.h"
#include "util.h"

Decoded *recieve_decoded(int sockfd) {
	void *header_bytes = malloc(HEADER_SIZE);

	ssize_t header_len = recv(sockfd, header_bytes, HEADER_SIZE, 0);
	if (header_len == -1) {
		return NULL;
	}

	Header *header = malloc(sizeof *header);
	decode_header(header, header_bytes);

	void *body_bytes = malloc(header->size);
	ssize_t body_len = recv(sockfd, body_bytes, header->size, 0);
	if (body_len == -1) {
		return NULL;
	}

	Decoded *decoded = malloc(sizeof *decoded);

	#if ENCODER == bencode
	bencode_init(decoded, body_bytes, header->size);
	#endif

	free(header_bytes);
	free(header);
	free(body_bytes);

	return decoded;
}
