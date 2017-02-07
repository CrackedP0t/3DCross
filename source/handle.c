#include <queue.h>
#include <stdlib.h>
#include <string.h>

#if defined(_3DS)
#include <sf2d.h>
#include <sfil.h>
#endif

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

static inline int handle_draw(int sockfd, Body *body) {
	size_t off = 0;
	int error;

	Bencode *obj = ben_decode2(body->string, body->len, &off, &error);

	if (error) {
		ben_free(obj);
		obj = NULL;
		return 1;
	}

	// name, wid, x, y, width, height, coding, data, packet_sequence, rowstride
	const char *coding = ben_str_val(ben_list_get(obj, 6));

	if (!strcmp(coding, "png")) {
		Chunk *chunk = NULL;

		LIST_FOREACH(chunk, &body->head, chunks) {
			if (chunk->index == 7) {
#if defined(_3DS)
				sf2d_texture *image = sfil_load_PNG_buffer(chunk->bytes, SF2D_PLACE_RAM);

				sf2d_start_frame(GFX_TOP, GFX_LEFT);

				sf2d_draw_texture(image, 0, 0);

				sf2d_end_frame();

				sf2d_swapbuffers();

				sf2d_free_texture(image);
#endif
				break;
			}
		}
	}

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
		const char *name = ben_str_val(ben_list_get(obj, 0));

		printf("recv: %s\n", name);

		if (!strcmp(name, "hello")) {
			res = handle_hello(sockfd, body);
		} else if (!strcmp(name, "new-window")) {
			res = handle_new_window(sockfd, body);
		} else if (!strcmp(name, "draw")) {
			res = handle_draw(sockfd, body);
		} else if (!strcmp(name, "ping")) {
			res = handle_ping(sockfd, body);
		} else if (!strcmp(name, "disconnect")) {
			res = handle_disconnect(sockfd, body);
		}

		if (res == 1) {
			puts("Error in handler");
		}
	} else {
		puts("Error in ben_decode2()");
		res = 1;
	}

	ben_free(obj);
	obj = NULL;

	return res;
}
