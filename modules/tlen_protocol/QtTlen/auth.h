/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AUTH_H
#define AUTH_H

//-----------------------------------------------------
// Kod zaczerpniety z libtlen
//-----------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 	typedef struct {
		unsigned long H[5];
		unsigned long W[80];
		int lenW;
		unsigned long sizeHi, sizeLo;
	}
	j_SHA_CTX;


	void shaInit (j_SHA_CTX * ctx);
	void shaUpdate (j_SHA_CTX * ctx, unsigned char *dataIn, int len);
	void shaFinal (j_SHA_CTX * ctx, unsigned char hashout[20]);
	void shaBlock (unsigned char *dataIn, int len,
		       unsigned char hashout[20]);

extern char *tlen_hash (const char *pass, const char *id);
#endif
