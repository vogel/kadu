/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PATH_CONVERSION_H
#define PATH_CONVERSION_H

#include <QtCore/QString>

#include "exports.h"

KADUAPI void printBacktrace(const QString &header = QString());

KADUAPI QString homePath();

/**
	Zmienia ścieżkę relatywną do katalogu z ustawieniami programu
	na ścieżkę bezwzgledną uwzględniając zmienne środowiskowe
	$HOME i $CONFIG_DIR
**/
KADUAPI QString profilePath(const QString &subpath = QString());

/**
	zwraca ścieżkę do pliku f
	jeżeli drugi parametr nie jest == 0, to funkcja próbuje najpierw ustalić
	ścieżkę na podstawie argv0, które ma być równe argv[0] oraz zmiennej PATH
**/
KADUAPI QString dataPath(const QString &f = QString(), const char *argv0 = 0);

KADUAPI QString libPath(const QString &f = QString());

KADUAPI QString webKitPath(const QString &path);

#endif // PATH_CONVERSION_H
