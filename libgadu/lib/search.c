/* $Id: search.c,v 1.16 2003/02/02 01:07:35 adrian Exp $ */

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
#include <stdio.h>
#include <errno.h>
#ifndef _AIX
#  include <string.h>
#endif
#include "libgadu.h"

/*
 * gg_search()
 *
 * ZE WZGLÊDU NA ZMIANY W PROTOKOLE, WYSZUKIWANIE T¡ METOD¡ NIE ZWRÓCI
 * ¯ADNYCH WYNIKÓW. NALE¯Y U¯YWAÆ FUNKCJI gg_search50(). FUNKCJA ZOSTA£A
 * WY£¡CZNIE DLA ZACHOWANIA ABI.
 */
struct gg_http *gg_search(const struct gg_search_request *r, int async)
{
	gg_debug(GG_DEBUG_MISC, "// gg_search() is obsolete. use gg_search50() instead!\n");
	errno = EINVAL;
	return NULL;
}

/*
 * gg_search_watch_fd()
 *
 * funkcja zosta³a wy³±cznie dla zachowania ABI.
 */
int gg_search_watch_fd(struct gg_http *h)
{
	gg_debug(GG_DEBUG_MISC, "// gg_search_watch_fd() is obsolete. use gg_search50() instead!\n");

	errno = EINVAL;
	return -1;
}

/*
 * gg_search_free()
 *
 * funkcja zosta³a wy³±cznie dla zachowania ABI.
 */
void gg_search_free(struct gg_http *h)
{
}

/*
 * gg_search_request_mode*()
 *
 * funkcje zosta³a wy³±cznie dla zachowania ABI.
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
 * funkcja zosta³a wy³±cznie dla zachowania ABI.
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
