/* $Id$ */

/*
 * g³upiutki przyk³ad, jak siê po³±czyæ, korzystaj±c z asynchronicznych
 * socketów. nic poza po³±czeniem nie robi. nie przejmuje siê b³êdami.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "libgadu.h"

int main()
{
	struct gg_login_params p;
	struct gg_session *sess;
	struct timeval tv;
	struct gg_event *e;
	fd_set rd, wd;
	int ret;

	gg_debug_level = ~0;
	
	memset(&p, 0, sizeof(p));
	p.uin = 123456;
	p.password = "dupa.8";
	p.async = 1;
	
	sess = gg_login(&p);

	for (;;) {
		FD_ZERO(&rd);
		FD_ZERO(&wd);

		if ((sess->check & GG_CHECK_READ))
			FD_SET(sess->fd, &rd);
		if ((sess->check & GG_CHECK_WRITE))
			FD_SET(sess->fd, &wd);

		tv.tv_sec = 10;
		tv.tv_usec = 0;
		
		ret = select(sess->fd + 1, &rd, &wd, NULL, &tv);
	
		if (!ret) {
			printf("timeout! wypad!\n");
			return 1;
		} else {
			if (sess && (FD_ISSET(sess->fd, &rd) || FD_ISSET(sess->fd, &wd))) {
				if (!(e = gg_watch_fd(sess))) {
					printf("zerfauo pouontshenie!\n");
					return 1;
				}
				if (e->type == GG_EVENT_CONN_SUCCESS) {
					printf("po³±czy³em siê\n");
					gg_free_event(e);
					gg_logoff(sess);
					gg_free_session(sess);
					return 1;
				}
				if (e->type == GG_EVENT_CONN_FAILED) {
					printf("errrror\n");
					gg_free_event(e);
					gg_logoff(sess);
					gg_free_session(sess);
					return 1;
				}
				gg_free_event(e);
			}
		}
	}
	
	return -1;
}

