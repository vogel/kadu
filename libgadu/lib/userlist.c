/* $Id: userlist.c,v 1.6 2002/09/22 19:23:37 chilek Exp $ */

/*
 *  (C) Copyright 2001-2002 Wojtek Kaniewski <wojtekka@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License Version
 *  2.1 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#ifndef _AIX
#  include <string.h>
#endif
#include <stdarg.h>
#include <ctype.h>
#include "config.h"
#include "libgadu.h"

/*
 * gg_userlist_get()
 *
 * wysy³a ¿±danie pobrania listy kontaktów z serwera.
 *
 *  - uin - numer
 *  - passwd - has³o
 *  - async - po³±czenie asynchroniczne
 *
 * zaalokowana struct gg_http, któr± po¼niej nale¿y zwolniæ
 * funkcj± gg_userlist_get_send(), albo NULL je¶li wyst±pi³ b³±d.
 */
struct gg_http *gg_userlist_get(uin_t uin, const char *passwd, int async)
{
	struct gg_http *h;
	char *form, *query, *__passwd;

	if (!passwd) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_get, NULL parameter\n");
		errno = EINVAL;
		return NULL;
	}

	if (!(__passwd = gg_urlencode(passwd))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_get, not enough memory for form fields\n");
		free(__passwd);
		errno = ENOMEM;
		return NULL;
	}
	
	if (!(form = gg_saprintf("FmNum=%ld&Pass=%s", uin, __passwd))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_get, not enough memory for form fields\n");
		free(__passwd);
		errno = ENOMEM;
		return NULL;
	}
	
	free(__passwd);
	
	gg_debug(GG_DEBUG_MISC, "=> userlist_get, %s\n", form);

        query = gg_saprintf(
		"Host: " GG_PUBDIR_HOST "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "User-Agent: " GG_HTTP_USERAGENT "\r\n"
                "Content-Length: %d\r\n"
                "Pragma: no-cache\r\n"
                "\r\n"
                "%s",
                strlen(form), form);

	free(form);

	if (!(h = gg_http_connect(GG_PUBDIR_HOST, GG_PUBDIR_PORT, async, "POST", "/appsvc/fmcontactsget.asp", query))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_get, gg_http_connect() failed mysteriously\n");
                free(query);
		return NULL;
	}

	h->type = GG_SESSION_USERLIST_GET;

	free(query);

	h->callback = gg_userlist_get_watch_fd;
	h->destroy = gg_userlist_get_free;
	
	if (!async)
		gg_pubdir_watch_fd(h);

	return h;
}

/*
 * gg_userlist_get_watch_fd()
 *
 * przy asynchronicznym ¶ci±ganiu listy kontaktów nale¿y wywo³aæ t± funkcjê
 * po zauwa¿eniu zmian na obserwowanym deskryptorze.
 *
 *  - h - struktura opisuj±ca po³±czenie
 *
 * je¶li wszystko posz³o dobrze to 0, inaczej -1. operacja bêdzie
 * zakoñczona, je¶li h->state == GG_STATE_DONE. je¶li wyst±pi jaki¶
 * b³±d, to bêdzie tam GG_STATE_ERROR i odpowiedni kod b³êdu w h->error.
 */
int gg_userlist_get_watch_fd(struct gg_http *h)
{
	if (!h) {
		errno = EINVAL;
		return -1;
	}

        if (h->state == GG_STATE_ERROR) {
                gg_debug(GG_DEBUG_MISC, "=> userlist_get, watch_fd issued on failed session\n");
                errno = EINVAL;
                return -1;
        }
	
	if (h->state != GG_STATE_PARSING) {
		if (gg_http_watch_fd(h) == -1) {
			gg_debug(GG_DEBUG_MISC, "=> userlist_get, http failure\n");
                        errno = EINVAL;
			return -1;
		}
	}

	if (h->state != GG_STATE_PARSING)
		return 0;

        h->state = GG_STATE_DONE;
	h->data = NULL;

	gg_debug(GG_DEBUG_MISC, "=> userlist_get, let's parse \"%s\"\n", h->body);

	if (!h->body || strncmp(h->body, "get_results:", 12)) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_get, error.\n");
		return 0;
	}

	h->data = strdup(h->body + 12);
	
	return 0;
}

/*
 * gg_userlist_get_free()
 *
 * zwalnia pamiêæ po pobieraniu listy kontaktów z serwera.
 *
 *  - h - zwalniana struktura
 */
void gg_userlist_get_free(struct gg_http *h)
{
	if (!h)
		return;
	free(h->data);
	gg_free_http(h);
}

/*
 * gg_userlist_put()
 *
 * wysy³a listê kontaktów na serwer.
 *
 *  - uin - numerek
 *  - passwd - has³o
 *  - contacts - lista kontaktów
 *  - async - ma byæ asynchronicznie
 *
 * zaalokowana struct gg_http, któr± po¼niej nale¿y zwolniæ
 * funkcj± gg_userlist_send(), albo NULL je¶li wyst±pi³ b³±d.
 */
struct gg_http *gg_userlist_put(uin_t uin, const char *passwd, const char *contacts, int async)
{
	struct gg_http *h;
	char *form, *query, *__passwd, *__contacts = NULL;

	if (!passwd || !contacts) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_put, NULL parameter\n");
		errno = EINVAL;
		return NULL;
	}

	if (!(__passwd = gg_urlencode(passwd)) || !(__contacts = gg_urlencode(contacts))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_put, not enough memory for form fields\n");
		free(__passwd);
		free(__contacts);
		errno = ENOMEM;
		return NULL;
	}
	
	if (!(form = gg_saprintf("FmNum=%ld&Pass=%s&Contacts=%s", uin, __passwd, __contacts))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_put, not enough memory for form fields\n");
		free(__passwd);
		free(__contacts);
		errno = ENOMEM;
		return NULL;
	}
	
	free(__passwd);
	free(__contacts);
	
	gg_debug(GG_DEBUG_MISC, "=> userlist_put, %s\n", form);

        query = gg_saprintf(
		"Host: " GG_PUBDIR_HOST "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "User-Agent: " GG_HTTP_USERAGENT "\r\n"
                "Content-Length: %d\r\n"
                "Pragma: no-cache\r\n"
                "\r\n"
                "%s",
                strlen(form), form);

	free(form);

	if (!(h = gg_http_connect(GG_PUBDIR_HOST, GG_PUBDIR_PORT, async, "POST", "/appsvc/fmcontactsput.asp", query))) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_put, gg_http_connect() failed mysteriously\n");
                free(query);
		return NULL;
	}

	h->type = GG_SESSION_USERLIST_PUT;

	free(query);

	h->callback = gg_userlist_put_watch_fd;
	h->destroy = gg_userlist_put_free;
	
	if (!async)
		gg_userlist_put_watch_fd(h);

	return h;
}

/*
 * gg_userlist_put_watch_fd()
 *
 * przy asynchronicznym wysy³aniu userlisty nale¿y wywo³aæ t± funkcjê
 * po zauwa¿eniu zmian na obserwowanym deskryptorze
 *
 *  - h - struktura opisuj±ca po³±czenie
 *
 * je¶li wszystko posz³o dobrze to 0, inaczej -1. operacja bêdzie
 * zakoñczona, je¶li h->state == GG_STATE_DONE. je¶li wyst±pi jaki¶
 * b³±d, to bêdzie tam GG_STATE_ERROR i odpowiedni kod b³êdu w h->error.
 */
int gg_userlist_put_watch_fd(struct gg_http *h)
{
	if (!h) {
		errno = EINVAL;
		return -1;
	}

        if (h->state == GG_STATE_ERROR) {
                gg_debug(GG_DEBUG_MISC, "=> userlist_put, watch_fd issued on failed session\n");
                errno = EINVAL;
                return -1;
        }
	
	if (h->state != GG_STATE_PARSING) {
		if (gg_http_watch_fd(h) == -1) {
			gg_debug(GG_DEBUG_MISC, "=> userlist_put, http failure\n");
                        errno = EINVAL;
			return -1;
		}
	}

	if (h->state != GG_STATE_PARSING)
		return 0;

        h->state = GG_STATE_DONE;
	h->data = NULL;

	gg_debug(GG_DEBUG_MISC, "=> userlist_put, let's parse \"%s\"\n", h->body);

	if (!h->body || strncmp(h->body, "put_success:", 12)) {
		gg_debug(GG_DEBUG_MISC, "=> userlist_put, error.\n");
		return 0;
	}

	h->data = (char*) 1;
	
	return 0;
}

/*
 * gg_userlist_put_free()
 *
 * zwalnia pamiêæ po wysy³aniu listy kontaktów na serwer.
 *
 *  - h - zwalniana struktura
 */
void gg_userlist_put_free(struct gg_http *h)
{
	gg_free_http(h);
}

/*
 * Local variables:
 * c-indentation-style: k&r
 * c-basic-offset: 8
 * indent-tabs-mode: notnil
 * End:
 *
 * vim: shiftwidth=8:
 */
