#include <errno.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include "encoder.h"
#include "packet.h"
#include "recieve.h"
#include "util.h"

static void *get_packet(int sockfd, Header *header, int *error) {
	*error = 0;

	void *header_bytes = malloc(HEADER_SIZE);

	ssize_t header_len = recv(sockfd, header_bytes, HEADER_SIZE, 0);
	if (header_len == -1) {
		*error = errno;
		free(header_bytes);
		header_bytes = NULL;
		return NULL;
	}

	decode_header(header, header_bytes);

	free(header_bytes);
	header_bytes = NULL;

	void *body_bytes = malloc(header->size);

	ssize_t total_len = 0;

	while (total_len < header->size) {
		ssize_t body_len = recv(sockfd, body_bytes + total_len, header->size - total_len, 0);
		if (body_len == -1) {
			*error = 1;
			free(body_bytes);
			body_bytes = NULL;
			return NULL;
		}

		total_len += body_len;
	}

	return body_bytes;
}

int recieve_body(int sockfd, Body *body) {
	body->string = NULL;
	body->len = -1;
	LIST_INIT(&body->head);

	Header *header = malloc(sizeof *header);
	int error = 0;
	void *body_bytes = get_packet(sockfd, header, &error);

	if (error) {
		free(header);
		free(body_bytes);
		header = body_bytes = NULL;
		return 1;
	}

	while (header->index > 0) {
		Chunk *chunk = malloc(sizeof *chunk);
		chunk->index = header->index;
		chunk->bytes = body_bytes;
		chunk->len = header->size;

		LIST_INSERT_HEAD(&body->head, chunk, chunks);

		free(header);
		header = NULL;

		header = malloc(sizeof *header);
		body_bytes = get_packet(sockfd, header, &error);

		if (error) {
			free(header);
			free(body_bytes);
			header = body_bytes = NULL;
			return 1;
		}
	}

	free(header);
	header = NULL;

	body->string = (char *)body_bytes;
	body->len = header->size;

	return 0;
}
