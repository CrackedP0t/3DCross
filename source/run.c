#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(_3DS)
#include <3ds.h>
#endif

#include "handle.h"
#include "hello.h"
#include "recieve.h"
#include "run.h"

#define SERVER_IP "192.168.0.21"
#define SERVER_PORT 14500

void error_start() {
	puts("An error has occured! Please report it.");

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

void error_end(int sockfd, int shouldEnd) {
	if (shouldEnd) {
		end(sockfd);
	}

	exit(0);
}

void error_run(int sockfd, int shouldEnd) {
	error_start();
	error_loop();
	error_end(sockfd, shouldEnd);
}

int start(int *sockfd_out) {
#if defined(_3DS)
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	Result result;
	result = socInit((u32*)memalign(0x1000, 0x128000), 0x128000);
	if (R_FAILED(result)) {
		gfxExit();
		puts("Error in socInit()");
		return 1;
	}
#endif

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	*sockfd_out = sockfd;
	if (sockfd == -1) {
#if defined(_3DS)
		socExit();
		gfxExit();
#endif
		perror("Error in socket()");
		return 1;
	}

	struct sockaddr_in server;

	uint32_t addr;
	int addr_res;

	do {
		addr_res = inet_pton(AF_INET, SERVER_IP, &addr);
	} while (addr_res != 1);

	server.sin_addr.s_addr = addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
		close(sockfd);
#if defined(_3DS)
		socExit();
		gfxExit();
#endif
		perror("Error in connect()");
		puts("Guess: the IP or port cannot be reached");
		return 1;
	}
	if (!send_hello(sockfd)) {
		close(sockfd);
#if defined(_3DS)
		socExit();
		gfxExit();
#endif
		puts("Error in send_hello()");
		return 1;
	}

	return 0;
}

int loop(int sockfd) {
#if defined(_3DS)
	while(aptMainLoop()) {
#elif defined(PC)
	while (1) {
#endif
		Body *body = malloc(sizeof *body);
		int error = recieve_body(sockfd, body);

		int res = 0;
		if (error) {
			puts("Error in recieve_body()");
			res = 1;
		} else {
			res = handle_packet(sockfd, body);
			if (res == -1) {
				puts("Disconnect without error");
			} else if (res == 1) {
				puts("Error in handle_packet()");
			}
		}

		Chunk *chunk = NULL;
		Chunk *next = NULL;
		LIST_FOREACH_SAFE(chunk, &body->head, chunks, next) {
			LIST_REMOVE(chunk, chunks);
			free(chunk);
			chunk = NULL;
		}

		free(body);
		body = NULL;

		if (res == -1) {
			return 0;
		} else if (res == 1) {
			return 1;
		}

#if defined(_3DS)
		hidScanInput();
		if(hidKeysDown() & KEY_START) {
			puts("Exit command recieved");
			return 0;
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
#elif defined(PC)
		usleep(16000);
#endif
	}

 	return 0;
}

int end(int sockfd) {
	int error = 0;

	if (close(sockfd) == -1) {
		if (errno != EBADF) {
			perror("Error in close()");
			error = 1;
		}
	}

#if defined(_3DS)
	Result result;
	result = socExit();
	if (R_FAILED(result)) {
		puts("Error in socExit()");
		error = 1;
	}

	gfxExit();
#endif

	return error;
}

void run() {
	int sockfd;

	if (start(&sockfd)) {
		error_run(sockfd, 0);
	} else {
		if (loop(sockfd)) {
			error_run(sockfd, 1);
		} else {
			if (end(sockfd)) {
				error_run(sockfd, 0);
			}
		}
	}
}
