/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
		ThemeManager(parent)
{
}

IconThemeManager::~IconThemeManager()
{
}

QStringList IconThemeManager::defaultThemePathes()
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

QString IconThemeManager::getThemeName(const QString &themePath)
{
	int lastSlash = themePath.lastIndexOf('/');
	if (-1 == lastSlash)
		return "";
	return themePath.mid(lastSlash + 1);
}
