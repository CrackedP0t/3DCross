#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if defined(_3DS)
#include <3ds.h>
#endif
#include <zlib.h>

#include "draw.h"
#include "encoder.h"
#include "handle.h"
#include "send.h"
#include "stb_image.h"
#include "util.h"

static int fb_number = 1;

static int old_width, old_height = -1;

DEF_HANDLER(handle_hello) {
	return 0;
}

DEF_HANDLER(handle_new_window) {
	//name, wid, x, y, w, h, info

	old_width = ben_int_val(ben_list_get(obj, 4));
	old_height = ben_int_val(ben_list_get(obj, 5));

	Bencode *response = ben_list();

	ben_list_append(response, ben_str("map-window"));
	BEN_GETCLAPP(response, obj, 1);
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(TOP_W));
	ben_list_append(response, ben_int(TOP_H));
	BEN_GETCLAPP(response, obj, 7);

	send_bencode(sockfd, response);

	ben_free(response);
	response = NULL;

	return 0;
}

DEF_HANDLER(handle_draw) {
	struct timeval start_tv;
	gettimeofday(&start_tv, NULL);
	uint64_t start_usec = (1000000 * start_tv.tv_sec) + start_tv.tv_usec;

	// name, wid, x, y, width, height, coding, data, packet_sequence, rowstride

	int img_x = ben_int_val(ben_list_get(obj, 2));
	int img_y = ben_int_val(ben_list_get(obj, 3));
	int width = ben_int_val(ben_list_get(obj, 4));
	int height = ben_int_val(ben_list_get(obj, 5));

	const char *coding = ben_str_val(ben_list_get(obj, 6));

	const unsigned char *data = (const unsigned char *)ben_str_val(ben_list_get(obj, 7));
	int len = ben_str_len(ben_list_get(obj, 7));

	unsigned char *image = NULL;
	int free_image = 0;
	if (!strcmp(coding, "png")) {
		int x_out, y_out, channels;
		image = stbi_load_from_memory((unsigned char *)data, len, &x_out, &y_out, &channels, 3);

		if (!image) {
			printf("Error in stbi_load(): %s\n", stbi_failure_reason());

			return 1;
		}

		free_image = 1;
	} else if (!strcmp(coding, "rgb24")) {
		unsigned long end_len = 3 * width * height;
		Bytef *dest = malloc(end_len);

		int zerr = uncompress(dest, &end_len, data, len);

		if (zerr == Z_OK) {
			image = dest;
			free_image = 1;
		} else {
			free(dest);
			dest = NULL;

			if (zerr == Z_DATA_ERROR) {
				image = (unsigned char *)data;
				free_image = 0;
			} else {
				puts("Error in uncompress()");
				return 1;
			}
		}
	}

#if defined(_3DS)
	if (image) {
		u8* fb = gfxTopLeftFramebuffers[fb_number];

		int region_width = MIN(int, width, TOP_W);
		int region_height = MIN(int, height, TOP_H);

		for (int y = 0; y < region_height && img_y + y < TOP_H; y++) {
			for (int x = 0; x < region_width && img_x + x < TOP_W; x++) {
				int pos_img = 3 * (x + y * width);

				set_3ds_fb_top(fb, img_x + x, img_y + y
						   , image[pos_img + 0], image[pos_img + 1], image[pos_img + 2]);
			}
		}

		gfxFlushBuffers();
	}
#endif

	// The image can be on the stack or heap
	if (free_image) {
		free(image);
		image = NULL;
	}

	struct timeval end_tv;
	gettimeofday(&end_tv, NULL);
	uint64_t end_usec = (1000000 * end_tv.tv_sec) + end_tv.tv_usec;

	uint64_t total_usec = end_usec - start_usec;

	Bencode *response = ben_list();

	ben_list_append(response, ben_str("damage-sequence"));

	BEN_GETCLAPP(response, obj, 8);
	BEN_GETCLAPP(response, obj, 1);
	BEN_GETCLAPP(response, obj, 2);
	BEN_GETCLAPP(response, obj, 3);
	ben_list_append(response, ben_int(total_usec));

	send_bencode(sockfd, response);

	ben_free(response);
	response = NULL;

	return 0;
}

DEF_HANDLER(handle_window_resized) {
	int new_width = ben_int_val(ben_list_get(obj, 2));
	int new_height = ben_int_val(ben_list_get(obj, 3));

#if defined(_3DS)
	u8* fb = gfxTopLeftFramebuffers[fb_number];

	for (int y = new_height; y < old_height && y < TOP_H; y++) {
		for (int x = 0; x < old_width && x < TOP_W; x++) {
			set_3ds_fb_top(fb, x, y, 0, 0, 0);
		}
	}

	for (int x = new_width; x < old_width && x < TOP_W; x++) {
		for (int y = 0; y < old_height && y < TOP_H; y++) {
			set_3ds_fb_top(fb, x, y, 0, 0, 0);
		}
	}
#endif

	old_width = new_width;
	old_height = new_height;

	return 0;
}

DEF_HANDLER(handle_ping) {
	Bencode *response = ben_list();

	ben_list_append(response, ben_str("ping_echo"));

	BEN_GETCLAPP(response, obj, 1);
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(0));
	ben_list_append(response, ben_int(-1));

	send_bencode(sockfd, response);

	ben_free(response);
	response = NULL;

	return 0;
}

DEF_HANDLER(handle_disconnect) {
	return -1;
}

int handle_packet(int sockfd, Bencode *obj) {
	int res = 0;

	const char *name = ben_str_val(ben_list_get(obj, 0));

	printf("recv: %s\n", name);

	if (!strcmp(name, "hello")) {
		res = handle_hello(sockfd, obj);
	} else if (!strcmp(name, "new-window")) {
		res = handle_new_window(sockfd, obj);
	} else if (!strcmp(name, "draw")) {
		res = handle_draw(sockfd, obj);
	} else if (!strcmp(name, "window-resized")) {
		res = handle_window_resized(sockfd, obj);
	} else if (!strcmp(name, "ping")) {
		res = handle_ping(sockfd, obj);
	} else if (!strcmp(name, "disconnect")) {
		res = handle_disconnect(sockfd, obj);
	}

	if (res == 1) {
		printf("Error in handler for %s\n", name);
	}

	return res;
}
