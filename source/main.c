#define STBI_ONLY_PNG
#define STBI_FAILURE_USERMSG

#include "run.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
	run();

	return 0;
}
