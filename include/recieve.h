#pragma once

#include <stdint.h>
#include <sys/socket.h>

#include "encoder.h"
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

int recieve_bencode(int sockfd, Bencode **body);
