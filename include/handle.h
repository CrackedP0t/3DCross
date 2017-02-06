#pragma once

#include <stdarg.h>

#include "recieve.h"

int handle_packet(int sockfd, Body *decoded);
