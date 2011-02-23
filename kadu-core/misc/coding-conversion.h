/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CODING_CONVERSION_H
#define CODING_CONVERSION_H

#include "exports.h"

class QByteArray;
class QString;
class QTextCodec;

extern KADUAPI QTextCodec *codec_cp1250;
extern KADUAPI QTextCodec *codec_latin2;

KADUAPI QString cp2unicode(const QByteArray &);
KADUAPI QByteArray unicode2cp(const QString &);
KADUAPI QString latin2unicode(const QByteArray &);
KADUAPI QByteArray unicode2latin(const QString &);
KADUAPI QString unicode2std(const QString &);

// TODO: why not use normal QUrl::encode ?
//zamienia polskie znaki na format latin2 "url" (czyli do postaci %XY)
KADUAPI QString unicode2latinUrl(const QString &buf);
//zamienia kodowanie polskich znak�w przekonwertowane z utf-8 przy pomocy QUrl::encode na kodowanie latin-2
KADUAPI QString unicodeUrl2latinUrl(const QString &buf);

#endif // CODING_CONVERSION_H
