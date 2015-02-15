/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_MISC_H
#define KADU_MISC_H

#include "exports.h"

class QChar;
class QRect;
class QString;
class QWidget;
template<typename T> class QList;

KADUAPI QString replacedNewLine(const QString &text, const QString &newLineText);

KADUAPI QString pwHash(const QString &text);

KADUAPI QRect properGeometry(const QRect &rect);

QRect stringToRect(const QString &value, const QRect *def = 0);
QString rectToString(const QRect& rect);

QList<int> stringToIntList(const QString &in);
QString intListToString(const QList<int> &in);

KADUAPI QString fixFileName(const QString &path, const QString &fn);

KADUAPI QChar extractLetter(QChar c);

#endif // MISC_H
