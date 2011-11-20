/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include "misc/path-conversion.h"

#include "icon-theme-manager.h"

IconThemeManager::IconThemeManager(QObject *parent) :
		ThemeManager(false, parent)
{
}

IconThemeManager::~IconThemeManager()
{
}

QStringList IconThemeManager::defaultThemePaths()
{
	QStringList result = getSubDirs(dataPath("kadu/themes/icons"));
	result += getSubDirs(profilePath("icons"));

	return result;
}

bool IconThemeManager::isValidThemePath(const QString &themePath)
{
	QString kaduIconFileName = themePath + "/kadu_icons/64x64/kadu.png";
	QFileInfo kaduIconFile(kaduIconFileName);

	return kaduIconFile.exists();
}
