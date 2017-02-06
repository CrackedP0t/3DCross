#pragma once

#include <stddef.h>

#include "encoder.h"

int send_packet(int sockfd, void *packet, size_t size);

int send_both(int sockfd, void *body, size_t body_size);

int send_bencode(int sockfd, Bencode *obj);
