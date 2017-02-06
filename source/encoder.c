#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"

#if encoder == bencode
/*int extract_bencode(bencode_t *out, ...) {
	va_list argp;

	va_start(argp, out);

	int res = vextract_bencode(out, argp);

	va_end(argp);

	return res;
}

int vextract_bencode(bencode_t *out, va_list argp) {
	int go = 1;
	do {
		char next = va_arg(argp, int);

		switch (next) {
		case 'l':
			if (!bencode_is_list(out)) {
				return 0;
			}

			bencode_t temp;
			unsigned int index = va_arg(argp, unsigned int);

			for (unsigned int i = 0; i <= index; i++) {
				int res = bencode_list_get_next(out, &temp);

				if (!res) {
					return 0;
				}
			}

			*out = temp;
			break;
		case 'd':
			if (!bencode_is_dict(out)) {
				return 0;
			}
			break;
		case 'e':
			go = 0;
			break;
		};
	} while (go);

	return 1;
	}*/
#endif
