#pragma once

#include <stdlib.h>

#define TOP_W 400
#define TOP_H 240
#define BOT_W 320
#define BOT_H 200

#define TOP_AW 240
#define TOP_AH 400
#define BOT_AW 200
#define BOT_AH 320

// From http://stackoverflow.com/q/3437404/3084760
#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

#define ENSURE_int(i)   _Generic((i), int:   (i))
#define ENSURE_float(f) _Generic((f), float: (f))

#define MAX(type, x, y) \
	(type)GENERIC_MAX(ENSURE_##type(x), ENSURE_##type(y))
#define MIN(type, x, y) \
	(type)GENERIC_MIN(ENSURE_##type(x), ENSURE_##type(y))

void print_bytes(const void *object, size_t size);
