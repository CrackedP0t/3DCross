#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _3DS
#include <3ds.h>
#endif

#include "encoder.h"
#include "hello.h"
#include "packet.h"
#include "recieve.h"
#include "util.h"

#define SERVER_IP "192.168.0.21"
#define SERVER_PORT 14500

int main(int argc, char **argv) {
	#ifdef _3DS
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	socInit((u32*)memalign(0x1000, 0x128000), 0x128000);
	#endif

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sockfd == -1) {
		perror("error in socket()");
		goto loop;
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
		perror("error in connect()");
		goto loop;
	}

	int hellores = send_hello(sockfd);

	if (hellores == -1) {
		perror("Error in send_hello");
		goto loop;
	}

	Decoded *data = recieve_decoded(sockfd);

	//Now to handle the packets

	free(data);

	close(sockfd);

 loop:
	#ifdef _3DS
	while(aptMainLoop()) {
		hidScanInput();
		if(hidKeysDown() & KEY_START) {
			break;
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	socExit();

	gfxExit();
	#endif

	#ifdef PC
	while (1);
	#endif

	return 0;
}
