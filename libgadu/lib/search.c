/* $Id: search.c,v 1.10 2002/11/16 17:37:27 chilek Exp $ */

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
 * gg_search()
 *
 * rozpoczyna szukanie u¿ytkowników. informacje o tym, czego dok³adnie szukamy
 * s± zawarte w struct gg_search_request. ze wzglêdu na specyfikê ich
 * przeszukiwarki, niektórych pól nie mo¿na mieszaæ. s± oznaczone w libgadu.h
 * jako osobne tryby
 *
 *  - r - informacja o tym, czego szukamy
 *  - async - po³±czenie asynchroniczne
 *
 * zaalokowana struct gg_http, któr± po¼niej nale¿y zwolniæ
 * funkcj± gg_search_free(), albo NULL je¶li wyst±pi³ b³±d.
 */
struct gg_http *gg_search(const struct gg_search_request *r, int async)
{
	struct gg_http *h;
	char *form, *query;
	int mode = -1, gender;
	char __suffix[100];		/* XXX g³upio wygl±da */

	if (!r) {
		errno = EINVAL;
		return NULL;
	}
	
	gg_debug(GG_DEBUG_MISC, "// gg_search()\n");

	strcpy(__suffix, (r->active) ? "&ActiveOnly=" : "");

	if (r->start)
		sprintf(__suffix + strlen(__suffix), "&Start=%d", r->start & 0x7fffffff);
		
	if (r->nickname || r->first_name || r->last_name || r->city || r->gender || r->min_birth || r->max_birth)
		mode = 0;

	if (r->email) {
		if (mode != -1) {
			errno = EINVAL;
			return NULL;
		}
		mode = 1;
	}

	if (r->phone) {
		if (mode != -1) {
			errno = EINVAL;
			return NULL;
		}
		mode = 2;
	}

	if (r->uin) {
		if (mode != -1) {
			errno = EINVAL;
			return NULL;
		}
		mode = 3;
	}

	if (mode == -1) {
		errno = EINVAL;
		return NULL;
	}

	gender = (r->gender == GG_GENDER_NONE) ? -1 : r->gender;

	if (mode == 0) {
		char *__first_name, *__last_name, *__nickname, *__city;

		__first_name = gg_urlencode(r->first_name);
		__last_name = gg_urlencode(r->last_name);
		__nickname = gg_urlencode(r->nickname);
		__city = gg_urlencode(r->city);

		if (!__first_name || !__last_name || !__nickname || !__city) {
			free(__first_name);
			free(__last_name);
			free(__nickname);
			free(__city);
			gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for form fields\n");
			return NULL;
		}

		form = gg_saprintf("Mode=0&FirstName=%s&LastName=%s&Gender=%d&NickName=%s&City=%s&MinBirth=%d&MaxBirth=%d%s", __first_name, __last_name, gender, __nickname, __city, r->min_birth, r->max_birth, __suffix);

		free(__first_name);
		free(__last_name);
		free(__nickname);
		free(__city);

	} else if (mode == 1) {
		char *__email = gg_urlencode(r->email);

		if (!__email) {
			gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for form fields\n");
			return NULL;
		}

		form = gg_saprintf("Mode=1&Email=%s%s", __email, __suffix);

		free(__email);

	} else if (mode == 2) {
		char *__phone = gg_urlencode(r->phone);

		if (!__phone) {
			gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for form fields\n");
			return NULL;
		}

		form = gg_saprintf("Mode=2&MobilePhone=%s%s", __phone, __suffix);

		free(__phone);

	} else
		form = gg_saprintf("Mode=3&UserId=%u%s", r->uin, __suffix);

	if (!form) {
		gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for form query\n");
		return NULL;
	}

	gg_debug(GG_DEBUG_MISC, "=> search, %s\n", form);

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

	if (!(h = gg_http_connect(GG_PUBDIR_HOST, GG_PUBDIR_PORT, async, "POST", "/appsvc/fmpubquery2.asp", query))) {
		gg_debug(GG_DEBUG_MISC, "=> search, gg_http_connect() failed mysteriously\n");
		free(query);
		free(h);
		return NULL;
	}

	h->type = GG_SESSION_SEARCH;

	free(query);

	if (!async)
		gg_search_watch_fd(h);
	
	return h;
}

/*
 * gg_search_watch_fd()
 *
 * przy asynchronicznym wyszukiwaniu nale¿y wywo³aæ t± funkcjê przy
 * zmianach na obserwowanym deskryptorze.
 *
 *  - h - struktura opisuj±ca po³±czenie
 *
 * je¶li wszystko posz³o dobrze to 0, inaczej -1. przeszukiwanie bêdzie
 * zakoñczone, je¶li f->state == GG_STATE_DONE. je¶li wyst±pi jaki¶
 * b³±d, to bêdzie tam GG_STATE_ERROR i odpowiedni kod b³êdu w f->error.
 */
int gg_search_watch_fd(struct gg_http *h)
{
        struct gg_search *s;
	char *buf;

	if (!h) {
		errno = EINVAL;
		return -1;
	}

	gg_debug(GG_DEBUG_MISC, "// gg_search_watch_fd()\n");

        if (h->state != GG_STATE_PARSING) {
		if (gg_http_watch_fd(h) == -1) {
			gg_debug(GG_DEBUG_MISC, "=> search, http failure\n");
			return -1;
		}
	}
	
        if (h->state != GG_STATE_PARSING)
		return 0;

	buf = h->body;

	h->state = GG_STATE_DONE;

	gg_debug(GG_DEBUG_MISC, "=> search, let's parse\n");

        if (!(h->data = s = malloc(sizeof(struct gg_search)))) {
                gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for results\n");
                return -1;
        }

	s->count = 0;
	s->results = NULL;
	
	if (!gg_get_line(&buf)) {
		gg_debug(GG_DEBUG_MISC, "=> search, can't read the first line\n");
		return 0;
	}

	for (;;) {
		char *line[8];
		int i;

		for (i = 0; i < 8; i++) {
			if (!(line[i] = gg_get_line(&buf))) {
				gg_debug(GG_DEBUG_MISC, "=> search, can't read line %d of this user\n", i + 1);
				break;
			}
			gg_debug(GG_DEBUG_MISC, "=> search, line %i \"%s\"\n", i, line[i]);
		}

		if (i < 8)
			break;

		if (!(s->results = realloc(s->results, (s->count + 1) * sizeof(struct gg_search_result)))) {
			gg_debug(GG_DEBUG_MISC, "=> search, not enough memory for results (non critical)\n");
			return 0;
		}

		s->results[s->count].active = atoi(line[0]);
		s->results[s->count].uin = (strtol(line[1], NULL, 0));
		s->results[s->count].first_name = strdup(line[2]);
		s->results[s->count].last_name = strdup(line[3]);
		s->results[s->count].nickname = strdup(line[4]);
		s->results[s->count].born = atoi(line[5]);
		s->results[s->count].gender = atoi(line[6]);
		s->results[s->count].city = strdup(line[7]);

		s->count++;
	}

	gg_debug(GG_DEBUG_MISC, "=> search, done (%d entries)\n", s->count);

	return 0;
}

/*
 * gg_search_free()
 *
 * zwalnia pamiêæ po wyszukiwaniu.
 *
 *  - h - zwalniana struktura
 */
void gg_search_free(struct gg_http *h)
{
	struct gg_search *s;
	int i;

	if (!h)
		return;

	if ((s = h->data)) {
		for (i = 0; i < s->count; i++) {
			free(s->results[i].first_name);
			free(s->results[i].last_name);
			free(s->results[i].nickname);
			free(s->results[i].city);
		}
		free(s);
	}

	gg_http_free(h);
}

/*
 * gg_search_request_mode*()
 *
 * funkcje pozwalaj±ce w wygodny sposób tworzyæ struktury zawieraj±ce
 * kryteria wyszukiwania.
 *
 *  - nickname - pseudonim
 *  - first_name - imiê
 *  - last_name - nazwisko
 *  - city - miasto
 *  - gender - p³eæ (GG_GENDER_UNKNOWN, GG_GENDER_MALE, GG_GENDER_FEMALE)
 *  - min_birth - minimalna data urodzenia
 *  - max_birth - maksymalna data urodzenia
 *  - email - adres e-mail
 *  - phone - numer telefonu
 *  - uin - numer
 *  - active - szukaj tylko aktywnych
 *  - start - zacznij od podanego wyniku (dolne 31 bitów)
 *
 * wszystkie zwracaj± adres statycznego bufora, który mo¿na wykorzystaæ
 * do nakarmienia funkcji gg_search().
 */
const struct gg_search_request *gg_search_request_mode_0(char *nickname, char *first_name, char *last_name, char *city, int gender, int min_birth, int max_birth, int active, int start)
{
	static struct gg_search_request r;

	memset(&r, 0, sizeof(r));
	r.nickname = nickname;
	r.first_name = first_name;
	r.last_name = last_name;
	r.city = city;
	r.gender = gender;
	r.min_birth = min_birth;
	r.max_birth = max_birth;
	r.active = active;
	r.start = start;

	return &r;
}

const struct gg_search_request *gg_search_request_mode_1(char *email, int active, int start)
{
	static struct gg_search_request r;

	memset(&r, 0, sizeof(r));
	r.email = email;
	r.active = active;
	r.start = start;

	return &r;
}

const struct gg_search_request *gg_search_request_mode_2(char *phone, int active, int start)
{
	static struct gg_search_request r;

	memset(&r, 0, sizeof(r));
	r.phone = phone;
	r.active = active;
	r.start = start;

	return &r;
}

const struct gg_search_request *gg_search_request_mode_3(uin_t uin, int active, int start)
{
	static struct gg_search_request r;

	memset(&r, 0, sizeof(r));
	r.uin = uin;
	r.active = active;
	r.start = start;

	return &r;
}

/*
 * gg_search_request_free()
 *
 * zwalnia pamiêæ po struct gg_search_request.
 *
 *  - r - zwalniana struktura.
 */
void gg_search_request_free(struct gg_search_request *r)
{
	if (!r)
		return;
	if (r->first_name)
		free(r->first_name);
	if (r->last_name)
		free(r->last_name);
	if (r->nickname)
		free(r->nickname);
	if (r->city)
		free(r->city);
	if (r->email)
		free(r->email);
	if (r->phone)
		free(r->phone);
	free(r);
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
