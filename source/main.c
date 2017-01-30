#include <stdio.h>

#include <3ds.h>

int main(int argc, char **argv) {
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("Hello, world!\n");

	while(aptMainLoop()) {
		hidScanInput();
		if(hidKeysDown() & KEY_START) {
			break;
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
