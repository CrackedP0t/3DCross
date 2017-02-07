#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(_3DS)
#include <3ds.h>
#include <sf2d.h>
#endif

#include "connect.h"
#include "handle.h"
#include "hello.h"
#include "recieve.h"
#include "run.h"

void error_start() {
	puts("An error has occured!");
	puts("If it wasn't your fault, please report it!");

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
	if (!sf2d_init()) {
		puts("Error in sf2d_init()");
		return 1;
	}

	consoleInit(GFX_BOTTOM, NULL);

	Result result;
	result = socInit((u32*)memalign(0x1000, 0x128000), 0x128000);
	if (R_FAILED(result)) {
		puts("Error in socInit()");
		sf2d_fini();
		return 1;
	}
#endif

	return 0;
}

int loop() {
	int connected = 0;
	int sockfd = -1;
#if defined(_3DS)
	while(aptMainLoop()) {
#elif defined(PC)
	while (1) {
#endif
		if (connected) {
			Body *body = malloc(sizeof *body);
			int res = recieve_body(sockfd, body);

			if (res == 1) {
				puts("Error in recieve_body()");
			} else if (!res) {
				res = handle_packet(sockfd, body);
				if (res == -1) {
					puts("Disconnect normally");
				} else if (res == 1) {
					puts("Error in handle_packet()");
				}
			}

			Chunk *chunk = NULL;
			Chunk *next = NULL;
			LIST_FOREACH_SAFE(chunk, &body->head, chunks, next) {
				LIST_REMOVE(chunk, chunks);
				free(chunk->bytes);
				free(chunk);
				chunk = NULL;
			}

			free(body->string);
			free(body);
			body = NULL;

			if (res == -1) {
				if (end_connect(sockfd)) {
					puts("Error in end_connect()");
					return 1;
				} else {
					connected = 0;
					sockfd = -1;
				}
			} else if (res == 1) {
				end_connect(sockfd);
				return 1;
			}
		} else {
			if (begin_connect(&sockfd)) {
				puts("Error in begin_connect()");
				return 1;
			} else {
				connected = 1;
			}
		}

#if defined(_3DS)
		hidScanInput();
		if(hidKeysDown() & KEY_START) {
			puts("Exit command recieved");
			if (connected && end_connect(sockfd)) {
				puts("Error in end_connect()");
				return 1;
			} else {
				return 0;
			}
		}
#endif
	}

	if (end_connect(sockfd)) {
		puts("Error in end_connect()");
		return 1;
	} else {
		return 0;
	}
}

int end() {
	int error = 0;

#if defined(_3DS)
	if (R_FAILED(socExit())) {
		puts("Error in socExit()");
		error = 1;
	}

	if (!sf2d_fini()) {
		puts("Error in sf2d_fini()");
		error = 1;
	}
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
