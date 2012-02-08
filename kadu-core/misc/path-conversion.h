/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QtGlobal>

#include "exports.h"

#ifdef Q_WS_X11
KADUAPI QString desktopFilePath();
#endif

KADUAPI QString homePath();

/**
	Zmienia ścieżkę relatywną do katalogu z ustawieniami programu
	na ścieżkę bezwzgledną uwzględniając zmienne środowiskowe
	$HOME i $CONFIG_DIR
**/
KADUAPI QString profilePath(const QString &subpath = QString());

/**
	zwraca ścieżkę do pliku f
**/
KADUAPI QString dataPath(const QString &p = QString());

KADUAPI QString pluginsLibPath(const QString &f = QString());

KADUAPI QString webKitPath(const QString &path);

#endif // PATH_CONVERSION_H
