#pragma once

#include <stdint.h>
#include <sys/socket.h>

#include "queue.h"

#define HEADER_WAIT 5
#define BODY_WAIT 1000

typedef struct Chunk_s {
	uint8_t index;
	void *bytes;
	size_t len;

	LIST_ENTRY(Chunk_s) chunks;
} Chunk;

typedef LIST_HEAD(ChunkHead_s, Chunk_s) ChunkHead;

typedef struct {
	char *string;
	size_t len;
	ChunkHead head;
} Body;

int recieve_body(int sockfd, Body *body);
