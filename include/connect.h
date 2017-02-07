#pragma once

#include <inttypes.h>

#define SERVER_IP "192.168.0.21"
#define SERVER_PORT 14500

typedef struct {
	uint32_t addr;
	int error;
} IPEnterRes;

int begin_connect(int *sockfd_out);

int end_connect(int sockfd);
