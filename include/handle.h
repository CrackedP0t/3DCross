#pragma once

#include <stdarg.h>

#if defined(_3DS)
#include <sf2d.h>
#endif

#include "encoder.h"
#include "recieve.h"

#define DEF_HANDLER(name) static int name(int sockfd, Bencode *obj)

int handle_packet(int sockfd, Bencode *obj);
