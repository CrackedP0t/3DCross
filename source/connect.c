#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#if defined(_3DS)
#include <3ds.h>
#endif

#include "connect.h"
#include "hello.h"

int begin_connect(int *sockfd_out) {
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	*sockfd_out = sockfd;
	if (sockfd == -1) {
		perror("Error in socket()");
		return 1;
	}

	struct sockaddr_in server;

	uint32_t addr;

	int pton_error = inet_pton(AF_INET, SERVER_IP, &addr);

	if (pton_error == 0) {
		puts("IP address invalid");
		close(sockfd);
		return 1;
	} else if (pton_error == -1) {
		perror("Error in inet_pton()");
		close(sockfd);
		return 1;
	}

	server.sin_addr.s_addr = addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
		perror("Error in connect()");
		puts("Guess: The IP or port cannot be reached");
		close(sockfd);
		return 1;
	}

	if (!send_hello(sockfd)) {
		puts("Error in send_hello()");
		close(sockfd);
		return 1;
	}

	return 0;
}

int end_connect(int sockfd) {
	if (close(sockfd) == -1 && errno != 0) {
		perror("Error in close()");
		return 1;
	}

	return 0;
}
