#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "encoder.h"
#include "packet.h"
#include "util.h"

static uint32_t swap_uint32( uint32_t val )
{
	val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
	return (val << 16) | (val >> 16);
}

void *make_header(uint8_t encoder, uint8_t compression, uint8_t index, uint32_t size) {
	uint8_t magic = 'P';
	uint32_t flipped_size = swap_uint32(size);

	void *header = malloc(HEADER_SIZE);

	memcpy(header + 0, &magic, 1);
	memcpy(header + 1, &encoder, 1);
	memcpy(header + 2, &compression, 1);
	memcpy(header + 3, &index, 1);
	memcpy(header + 4, &flipped_size, 4);

	return header;
}

void *make_packet(size_t *packet_size_out, void *header, void *body, size_t body_size) {
	size_t packet_size = HEADER_SIZE + body_size;

	void *packet = malloc(packet_size);

	memcpy(packet, header, HEADER_SIZE);

	void *body_location = packet + HEADER_SIZE;

	memcpy(body_location, body, body_size);

	*packet_size_out = packet_size;

	return packet;
}


void *make_both(size_t *packet_size_out, void *body, size_t body_size) {
	void *header = make_header(ENCODER_ID, 0, 0, (uint32_t)body_size);

	void *packet = make_packet(packet_size_out, header, body, body_size);

	free(header);

	return packet;
}

void decode_header(Header *header_out, void *bytes) {
	memcpy(&header_out->magic, bytes + 0, 1);
	memcpy(&header_out->encoder, bytes + 1, 1);
	memcpy(&header_out->compression, bytes + 2, 1);
	memcpy(&header_out->index, bytes + 3, 1);
	memcpy(&header_out->size, bytes + 4, 4);

	header_out->size = swap_uint32(header_out->size);
}
