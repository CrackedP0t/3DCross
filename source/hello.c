#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "encoder.h"
#include "send.h"
#include "packet.h"
#include "util.h"

static inline Bencode *create_caps() {
	Bencode *data = ben_list();

	Bencode *hello = ben_str("hello");

	ben_list_append(data, hello);

	Bencode *caps = ben_dict();

	Bencode *encodings = ben_list();
	ben_list_append(encodings, ben_str("png"));

	Bencode *encoders = ben_list();
	ben_list_append(encoders, ben_str("bencode"));

	ben_dict_set_by_str(caps, "bencode", ben_int(1));
	ben_dict_set_by_str(caps, "rencode", ben_int(0));
	ben_dict_set_by_str(caps, "encoders", encoders);
	ben_dict_set_by_str(caps, "encodings", encodings);
	ben_dict_set_by_str(caps, "keyboard", ben_int(0));

	// A capability is missing somewhere, so when another client joins and sends
	// the info-request packet the server errors and disconnects it
	ben_dict_set_by_str(caps, "share", ben_int(0));

	Bencode *size = ben_list();
	ben_list_append(size, ben_int(400));
	ben_list_append(size, ben_int(240));

	ben_dict_set_by_str(caps, "desktop_size", size);

	ben_dict_set_by_str(caps, "version", ben_str("1.0.1"));
	ben_dict_set_by_str(caps, "windows", ben_int(1));

	srand(time(NULL));

	// This should be a way to uniquely identify 3DSes, but this will work for now.
	// We do have to provide `uuid`, or else the server errors when sending `server-event`.
	char * uuid = malloc(50);
	sprintf(uuid, "%x%x%x%x", rand(), rand(), rand(), rand());

	ben_dict_set_by_str(caps, "uuid", ben_str(uuid));

	ben_list_append(data, caps);

	free(uuid);
	uuid = NULL;
	caps = hello = encodings = encoders = NULL;

	return data;
}

int send_hello(int sockfd) {
	Bencode *obj = create_caps();

	int sendres = send_bencode(sockfd, obj);

	ben_free(obj);
	obj = NULL;

	return sendres;
}
