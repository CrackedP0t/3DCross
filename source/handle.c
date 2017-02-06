#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "handle.h"
#include "send.h"
#include "util.h"

static inline int handle_hello(int sockfd, Body *body) {
	return 0;
}

static inline int handle_new_window(int sockfd, Body *body) {
	size_t off = 0;
	int error;

	Bencode *obj = ben_decode2(body->string, body->len, &off, &error);

	if (error) {
		ben_free(obj);
		obj = NULL;
		return 1;
	}

	Bencode *response = ben_list();

	ben_list_append(response, ben_str("map-window"));
	ben_list_append(response, ben_clone(ben_list_get(obj, 1)));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(TOP_W));
	ben_list_append(response, ben_int(TOP_H));
	ben_list_append(response, ben_clone(ben_list_get(obj, 7)));

	send_bencode(sockfd, response);

	ben_free(obj);
	ben_free(response);
	obj = response = NULL;

	return 0;
}

static inline int handle_ping(int sockfd, Body *body) {
	size_t off = 0;
	int error;

	Bencode *obj = ben_decode2(body->string, body->len, &off, &error);

	if (error) {
		ben_free(obj);
		obj = NULL;
		return 1;
	}

	Bencode *response = ben_list();

	ben_list_append(response, ben_str("ping_echo"));

	ben_list_append(response, ben_clone(ben_list_get(obj, 1)));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(-1));

	send_bencode(sockfd, response);

	ben_free(obj);
	ben_free(response);

	obj = response = NULL;

	return 0;
}

static inline int handle_disconnect(int sockfd, Body *body) {
	return -1;
}

int handle_packet(int sockfd, Body *body) {
	int res = 0;

	size_t off = 0;
	int error;

	Bencode *obj = ben_decode2(body->string, body->len, &off, &error);

	if (!error) {
		Bencode *name_ben = ben_clone(ben_list_get(obj, 0));

		size_t name_len = ben_str_len(name_ben) + 1; // For \0 at end
		const char *name_str = ben_str_val(name_ben);

		char *name = malloc(name_len + 1);
		memcpy(name, name_str, name_len + 1);

		printf("recv: %s\n", name);

		if (!strcmp(name, "hello")) {
			res = handle_hello(sockfd, body);
		} else if (!strcmp(name, "new-window")) {
			res = handle_new_window(sockfd, body);
		} else if (!strcmp(name, "ping")) {
			res = handle_ping(sockfd, body);
		} else if (!strcmp(name, "disconnect")) {
			res = handle_disconnect(sockfd, body);
		}

		if (res == 1) {
			puts("Error in handler");
		}

		ben_free(name_ben);
		free(name);
		name_ben = NULL;
		name = NULL;
	} else {
		puts("Error in ben_decode2()");
		res = 1;
	}

	ben_free(obj);
	obj = NULL;

	return res;
}
