/*
 *  Copyright (c) 2003 Piotr Domagalski <szalik@szalik.net>
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

#include <errno.h>
#include <limits.h>
#include <string.h>

#ifndef PATH_MAX
#  define PATH_MAX _POSIX_PATH_MAX
#endif

char *dirname(const char *path)
{
	static char buf[PATH_MAX];
	register const char *ptr;

	if (!path || !*path || !strchr(path, '/')) {
		strncpy(buf, ".", sizeof(buf) - 1);
		return buf;
	}

	for (ptr = path + strlen(path) - 1; ptr > path; ptr--)
		if (*ptr != '/')
			break;

	for (; ptr > path; ptr--)
		if (*ptr == '/')
			break;

	if (ptr > path) {

		for (; ptr > path; ptr--)
			if (*ptr != '/')
				break;

		if (ptr - path + 2 > sizeof(buf)) {
			errno = ENAMETOOLONG;
			return NULL;
		}

		strncpy(buf, path, ptr - path + 1);

	} else
		strncpy(buf, "/", sizeof(buf) - 1);

	return buf;
}
