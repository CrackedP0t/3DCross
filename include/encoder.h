#pragma once

#define ENCODER bencode

#if ENCODER == bencode
#include "bencodetools/bencode.h"
#include "bencode.h"

#define ENCODER_ID 0

typedef bencode_t Decoded;
#endif
