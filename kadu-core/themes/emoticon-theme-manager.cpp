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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include "misc/path-conversion.h"

#include "emoticon-theme-manager.h"

bool EmoticonThemeManager::containsEmotsTxt(const QString &dir)
{
	QString kaduIconFileName = dir + "/emots.txt";
	QFileInfo kaduIconFile(kaduIconFileName);

	return kaduIconFile.exists();
}

EmoticonThemeManager::EmoticonThemeManager(QObject *parent) :
		ThemeManager(true, parent)
{
}

EmoticonThemeManager::~EmoticonThemeManager()
{
}

QStringList EmoticonThemeManager::defaultThemePaths()
{
	QStringList result = getSubDirs(dataPath("kadu/themes/emoticons"));
	result += getSubDirs(profilePath("icons"));

	return result;
}

bool EmoticonThemeManager::isValidThemePath(const QString &themePath)
{
	if (containsEmotsTxt(themePath))
		return true;

	QDir themeDir(themePath);
	QFileInfoList subDirs = themeDir.entryInfoList(QDir::Dirs);

	foreach (const QFileInfo &subDirInfo, subDirs)
	{
		if (!subDirInfo.fileName().startsWith('.'))
			if (containsEmotsTxt(subDirInfo.canonicalFilePath()))
				return true;
	}

	return false;
}
