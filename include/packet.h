#pragma once

#include <inttypes.h>
#include <stdlib.h>

#include "encoder.h"

#define HEADER_SIZE 8

typedef struct {
	uint8_t magic;
	uint8_t encoder;
	uint8_t compression;
	uint8_t index;
 	uint32_t size;
} Header;

void *make_header(uint8_t encoder, uint8_t compression, uint8_t index, uint32_t size);

void *make_packet(size_t *packet_size_out, void *header, void *body, size_t body_size);

void *make_both(size_t *packet_size_out, void *body, size_t body_size);

void decode_header(Header *header, void *bytes);

Decoded *decode_body(void *data, size_t data_size);
