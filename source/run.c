#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(_3DS)
#include <3ds.h>
#endif

#include "connect.h"
#include "handle.h"
#include "hello.h"
#include "recieve.h"
#include "run.h"
#include "util.h"

void error_start() {
	puts("An error has occured!");
	puts("If this is a bug, please report it!");

#if defined(_3DS)
	puts("Press start to exit.");
#endif
}

void error_loop() {
#if defined(_3DS)
	while (aptMainLoop()) {
		hidScanInput();
		if(hidKeysDown() & KEY_START) {
			return;
		}
	}
#endif
}

void error_end(int shouldEnd) {
	if (shouldEnd) {
		end();
	}
}

void error_run(int shouldEnd) {
	error_start();
	error_loop();
	error_end(shouldEnd);
}

int start() {
#if defined(_3DS)
	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	Result result;
	result = socInit((u32*)memalign(0x1000, 0x128000), 0x128000);
	if (R_FAILED(result)) {
		puts("Error in socInit()");
		return 1;
	}
#endif

	return 0;
}

int loop() {
	int retval = 0;

	int connected = 0;
	int sockfd = -1;

#if defined(_3DS)
	puts("Press A to connect to " SERVER_IP);

	while(aptMainLoop()) {
#else
	while (1) {
#endif
#if defined(_3DS)
		hidScanInput();

		u32 heldButtons = hidKeysHeld();
		u32 downButtons = hidKeysDown();
		u32 upButtons = hidKeysUp();
#endif

		if (connected) {
			Bencode *obj = NULL;
			int res = recieve_bencode(sockfd, &obj);

			if (res == 1) {
				puts("Error in recieve_bencode()");
			} else if (!res) {
				res = handle_packet(sockfd, obj);
				if (res == -1) {
					puts("Disconnect normally");
				} else if (res == 1) {
					puts("Error in handle_packet()");
				}
			}

			ben_free(obj);
			obj = NULL;

			if (res == -1) {
				connected = 0;
				sockfd = -1;
				if (end_connect(sockfd)) {
					puts("Error in end_connect()");
					retval = 1;
					break;
				}

#if defined(_3DS)
				puts("Press A to connect to " SERVER_IP);
#endif
			} else if (res == 1) {
				retval = res;
				break;
			}
		} else {
#if defined(_3DS)
			if (downButtons & KEY_A) {
#endif
			if (begin_connect(&sockfd)) {
				puts("Couldn't connect");

#if defined(_3DS)
				puts("Press A to connect to " SERVER_IP);
#endif
			} else {
				puts("Successfully connected");
				connected = 1;
			}
#if defined(_3DS)
			}
#endif
		}

#if defined(_3DS)
		if(heldButtons & KEY_START) {
			puts("Exit command recieved");

			break;
		}

		gspWaitForVBlank();
#endif
	}

	if (connected && end_connect(sockfd)) {
		puts("Error in end_connect()");
		return 1;
	} else {
		return retval;
	}
}

int end() {
	int error = 0;

#if defined(_3DS)
	if (R_FAILED(socExit())) {
		puts("Error in socExit()");
		error = 1;
	}

	gfxExit();
#endif

	return error;
}

void run() {
	if (start()) {
		error_run(0);
	} else {
		if (loop()) {
			error_run(1);
		} else {
			if (end()) {
				error_run(0);
			}
		}
	}
}
