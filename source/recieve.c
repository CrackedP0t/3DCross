#include <errno.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>

#include "encoder.h"
#include "packet.h"
#include "queue.h"
#include "recieve.h"
#include "util.h"

static void clear_chunks(ChunkHead *head) {
	Chunk *chunk = NULL;
	Chunk *next = NULL;
	LIST_FOREACH_SAFE(chunk, head, chunks, next) {
		LIST_REMOVE(chunk, chunks);
		free(chunk->bytes);
		free(chunk);
		chunk = NULL;
	}
}

static void *get_packet(int sockfd, Header *header, int *error) {
	*error = 0;

	void *header_bytes = malloc(HEADER_SIZE);

	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN;

	int ev = poll(&pfd, 1, HEADER_WAIT);

	switch(ev) {
	case -1:
		perror("Error in header poll()");
		free(header_bytes);
		header_bytes = NULL;
		*error = 1;
		return NULL;

	case 0:
		free(header_bytes);
		header_bytes = NULL;
		*error = -2;
		return NULL;

	default:
		;
		ssize_t header_len = recv(sockfd, header_bytes, HEADER_SIZE, 0);
		if (header_len == -1) {
			perror("Error in header recv()");
			free(header_bytes);
			header_bytes = NULL;
			*error = 1;
			return NULL;
		}
		break;
	}

	decode_header(header, header_bytes);

	free(header_bytes);
	header_bytes = NULL;

	if (header->magic != 'P') {
		puts("Incorrect magic byte");
		puts("Guess: Not an Xpra server");
		*error = 1;
		return NULL;
	}

	if (header->size > 0xffffff) {
		puts("Header size too large");
		puts("Guess: Not an Xpra server");
		*error = 1;
		return NULL;
	}

	void *body_bytes = malloc(header->size);

	ssize_t total_len = 0;

	while (total_len < header->size) {
		int ev = poll(&pfd, 1, BODY_WAIT);

		switch(ev) {
		case -1:
			perror("Error in body poll()");
			free(body_bytes);
			body_bytes = NULL;
			*error = 1;
			return NULL;

		case 0:
			puts("Timeout in body poll()");
			puts("Guess: Server no longer connected");
			free(body_bytes);
			body_bytes = NULL;
			*error = 1;
			return NULL;

		default:
			;
			ssize_t body_len = recv(sockfd, body_bytes + total_len, header->size - total_len, 0);
			if (body_len == -1) {
				perror("Error in body recv()");
				free(body_bytes);
				body_bytes = NULL;
				*error = 1;
				return NULL;
			}

			total_len += body_len;
		}
	}

	return body_bytes;
}

int recieve_bencode(int sockfd, Bencode **obj) {
	ChunkHead head;
	LIST_INIT(&head);

	Header header;
	int error = 0;
	void *body_bytes = get_packet(sockfd, &header, &error);

	if (error == 1) {
		free(body_bytes);
		body_bytes = NULL;
		puts("Error in get_packet()");
		return 1;
	} else if (error == -2) {
		free(body_bytes);
		body_bytes = NULL;
		return -2;
	}

	while (header.index > 0) {
		Chunk *chunk = malloc(sizeof *chunk);
		chunk->index = header.index;
		chunk->bytes = body_bytes;
		chunk->len = header.size;

		LIST_INSERT_HEAD(&head, chunk, chunks);

		body_bytes = get_packet(sockfd, &header, &error);

		if (error) {
			clear_chunks(&head);
			free(body_bytes);
			body_bytes = NULL;
			puts("Error in get_packet()");
			return 1;
		}
	}

	size_t off = 0;
	*obj = ben_decode2(body_bytes, header.size, &off, &error);

	free(body_bytes);
	body_bytes = NULL;

	size_t obj_len = ben_list_len(*obj);

	Chunk *chunk = NULL;
	LIST_FOREACH(chunk, &head, chunks) {
		if (chunk->index > obj_len - 1) {
			puts("Chunk index invalid");
			clear_chunks(&head);
			return 1;
		} else {
			ben_list_set(*obj, chunk->index, ben_bytes(chunk->bytes, chunk->len));
		}
	}

	clear_chunks(&head);

	if (error != BEN_OK) {
		printf("Error in ben_decode2(): %s", ben_strerror(error));
		ben_free(*obj);
		*obj = NULL;
		return 1;
	}
	return 0;
}
