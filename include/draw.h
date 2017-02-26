#pragma once

#if defined(_3DS)
#include <3ds.h>
#endif

#include "util.h"

#if defined(_3DS)
// This function accounts for the 3DS' rotation;
// All (x, y) coords in this program should be in landscape
inline void set_3ds_fb_top(u8* fb, int x, int y, u8 r, u8 g, u8 b) {
	int pos_3ds = 3 * ((TOP_H - 1 - y) + x * TOP_H);

	fb[pos_3ds + 0] = b;
	fb[pos_3ds + 1] = g;
	fb[pos_3ds + 2] = r;
}

inline void set_3ds_fb_bot(u8* fb, int x, int y, u8 r, u8 g, u8 b) {
	int pos_3ds = 3 * ((BOT_H - 1 - y) + x * BOT_H);

	fb[pos_3ds + 0] = b;
	fb[pos_3ds + 1] = g;
	fb[pos_3ds + 2] = r;
}

inline void clear_screen_top(u8 *fb, u8 r, u8 g, u8 b) {
	for (int x = 0; x < TOP_W; x++) {
		for (int y = 0; y < TOP_H; y++) {
			set_3ds_fb_top(fb, x, y, r, g, b);
		}
	}
}

inline void clear_screen_bot(u8 *fb, u8 r, u8 g, u8 b) {
	for (int x = 0; x < BOT_W; x++) {
		for (int y = 0; y < BOT_H; y++) {
			set_3ds_fb_top(fb, x, y, r, g, b);
		}
	}
}
#endif
