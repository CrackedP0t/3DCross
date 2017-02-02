#pragma once

#include <stddef.h>

int send_packet(int sockfd, void* packet, size_t size);
