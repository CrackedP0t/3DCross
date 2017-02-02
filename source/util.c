#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void print_bytes(const void *object, size_t size) {
	const unsigned char * const bytes = object;
	size_t i;

	printf("[ ");
	for(i = 0; i < size; i++)
		{
			printf("%02x ", bytes[i]);
		}
	printf("]\n");
}
