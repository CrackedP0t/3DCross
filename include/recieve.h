#pragma once

#include <sys/socket.h>

#include "encoder.h"

Decoded *recieve_decoded(int sockfd);
