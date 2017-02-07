#include <arpa/inet.h>
#include <errno.h>
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

	IPEnterRes *ip_res = malloc(sizeof *ip_res);
	*ip_res = (IPEnterRes){.addr = 0, .error = 0};

#if defined(_3DS)
	ip_res->error = !inet_pton(AF_INET, SERVER_IP, &ip_res->addr);
#else
	ip_res->error = !inet_pton(AF_INET, SERVER_IP, &ip_res->addr);
#endif

	if (ip_res->error) {
		perror("Error in inet_pton()");
		close(sockfd);
		free(ip_res);
		ip_res = NULL;
		return 1;
	}

	server.sin_addr.s_addr = ip_res->addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	free(ip_res);
	ip_res = NULL;

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
	int error = 0;

	if (close(sockfd) == -1) {
		if (errno != EBADF) {
			perror("Error in close()");
			error = 1;
		}
	}

	return error;
}
