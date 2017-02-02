#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "encoder.h"
#include "send.h"
#include "packet.h"
#include "util.h"

#if ENCODER == bencode
static void *create_caps(size_t *caps_size_out) {
	Bencode *data = ben_list();

	Bencode *hello = ben_str("hello");

	ben_list_append(data, hello);

	Bencode *caps = ben_dict();

	Bencode *encodings = ben_list();
	ben_list_append(encodings, ben_str("png"));

	Bencode *encoders = ben_list();
	ben_list_append(encoders, ben_str("bencode"));

	Bencode *window_states = ben_list();

	ben_dict_set_by_str(caps, "bencode", ben_int(1));
	ben_dict_set_by_str(caps, "rencode", ben_int(0));
	ben_dict_set_by_str(caps, "encoders", encoders);
	ben_dict_set_by_str(caps, "encodings", encodings);
	ben_dict_set_by_str(caps, "keyboard", ben_int(0));
	ben_dict_set_by_str(caps, "share", ben_int(1));
	ben_dict_set_by_str(caps, "version", ben_str("1.0.1"));
	ben_dict_set_by_str(caps, "windows", ben_int(1));

	ben_list_append(data, caps);

	void *body = ben_encode(caps_size_out, data);

	ben_free(data);

	data = caps = hello = encodings = encoders = window_states = NULL;

	return body;
}
#elif ENCODER == yaml
static void *create_caps(size_t *caps_size_out) {

}
#endif

int send_hello(int sockfd) {
	size_t *body_size_out = malloc(sizeof *body_size_out);

	void *body = create_caps(body_size_out);

	size_t body_size = *body_size_out;

	size_t *packet_size_out = malloc(sizeof *packet_size_out);

	void *packet = make_both(packet_size_out, body, body_size);

	size_t packet_size = *packet_size_out;

	int sendres = send_packet(sockfd, packet, packet_size);

	free(body_size_out);
	free(body);
	free(packet_size_out);
	free(packet);

	body_size_out = body = packet_size_out = packet = NULL;

	return sendres;
}
