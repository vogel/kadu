/*
 * przyk³ad prostego programu ³±cz±cego siê z serwerem i wysy³aj±cego
 * jedn± wiadomo¶æ.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "libgadu.h"

int main(int argc, char **argv)
{
	struct gg_session *sess;
	struct gg_event *e;
	struct gg_login_params p;

	if (argc < 5) {
		fprintf(stderr, "u¿ycie: %s <mójnumerek> <mojehas³o> <numerek> <wiadomo¶æ>\n", argv[0]);
		return 1;
	}

	gg_debug_level = 255;

	memset(&p, 0, sizeof(p));
	p.uin = atoi(argv[1]);
	p.password = argv[2];
	
	if (!(sess = gg_login(&p))) {
		printf("Nie uda³o siê po³±czyæ: %s\n", strerror(errno));
		gg_free_session(sess);
		return 1;
	}

	printf("Po³±czono.\n");

	if (gg_send_message(sess, GG_CLASS_MSG, atoi(argv[3]), argv[4]) == -1) {
		printf("Po³±czenie przerwane: %s\n", strerror(errno));
		gg_free_session(sess);
		return 1;
	}

	/* poni¿sz± czê¶æ mo¿na olaæ, ale poczekajmy na potwierdzenie */

	while (1) {
		if (!(e = gg_watch_fd(sess))) {
			printf("Po³±czenie przerwane: %s\n", strerror(errno));
			gg_logoff(sess);
			gg_free_session(sess);
			return 1;
		}

		if (e->type == GG_EVENT_ACK) {
			printf("Wys³ano.\n");
			gg_free_event(e);
			break;
		}

		gg_free_event(e);
	}

	gg_logoff(sess);
	gg_free_session(sess);

	return 0;
}
