/* $Id: search.c,v 1.6 2002/12/16 22:51:50 adrian Exp $ */

#include <stdio.h>
#include "libgadu.h"

#ifdef ASYNC

#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

void sigchld()
{
	wait(NULL);
	signal(SIGCHLD, sigchld);
}

#endif

int main()
{
	struct gg_http *h;
	struct gg_search *s;
	struct gg_search_request *r = gg_search_request_mode_0(NULL, "Ania", NULL, NULL, GG_GENDER_NONE, 0, 0, 0, 0);
	
	int i;

	gg_debug_level = 255;
	
#ifndef ASYNC

	if (!(h = gg_search(r, 0)))
		return 1;

#else

	signal(SIGCHLD, sigchld);

	if (!(h = gg_search(r, 1)))
		return 1;

        while (1) {
                fd_set rd, wr, ex;

                FD_ZERO(&rd);
                FD_ZERO(&wr);
                FD_ZERO(&ex);

                if ((h->check & GG_CHECK_READ))
                        FD_SET(h->fd, &rd);
                if ((h->check & GG_CHECK_WRITE))
                        FD_SET(h->fd, &wr);
                FD_SET(h->fd, &ex);

                if (select(h->fd + 1, &rd, &wr, &ex, NULL) == -1 || FD_ISSET(h->fd, &ex)) {
			if (errno == EINTR)
				continue;
			gg_free_search(h);
			perror("select");
			return 1;
		}

                if (FD_ISSET(h->fd, &rd) || FD_ISSET(h->fd, &wr)) {
			if (gg_search_watch_fd(h) == -1) {
				gg_free_search(h);
				fprintf(stderr, "no b³±d jak b³±d\n");
				return 1;
			}
			if (h->state == GG_STATE_ERROR) {
				gg_free_search(h);
				fprintf(stderr, "jaki¶tam b³±d\n");
				return 1;
			}
			if (h->state == GG_STATE_DONE)
				break;
		}
        }

#endif

	s = h->data;
	
	printf("count=%d\n", s->count);

	for (i = 0; i < s->count; i++) {
		printf("%ld: %s %s (%s), %d, %s\n", s->results[i].uin, s->results[i].first_name, s->results[i].last_name, s->results[i].nickname, s->results[i].born, s->results[i].city);
	}

	gg_free_search(h);

	return 0;
}

