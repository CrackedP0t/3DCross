#pragma once

#include "bencodetools/bencode.h"

#define ENCODER_ID 0

#define BEN_GETCLAPP(dest, src, index) \
	ben_list_append(dest, ben_clone(ben_list_get(src, index)))

typedef struct bencode Bencode;
