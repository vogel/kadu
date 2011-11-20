/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QStringList>

#include "theme-manager.h"

ThemeManager::ThemeManager(bool includeNone, QObject *parent) :
		QObject(parent), IncludeNone(includeNone), CurrentThemeIndex(-1)
{
}

ThemeManager::~ThemeManager()
{
}

int ThemeManager::getDefaultThemeIndex()
{
	for (int i = 0; i < Themes.size(); i++)
		if (Themes.at(i).name() == "default")
			return i;

	return -1;
}

QStringList ThemeManager::getSubDirs(const QString &dirPath)
{
	QDir dir(dirPath);
	QStringList subDirs = dir.entryList(QDir::Dirs);

	QStringList result;
	foreach (const QString &subDir, subDirs)
		if (!subDir.startsWith('.')) // ignore hidden, this, and parent
			result.append(dirPath + '/' + subDir);

	return result;
}

QString ThemeManager::getThemeName(const QString &themePath)
{
	QString path(themePath);
	// remove trailing slashes
#ifdef Q_WS_WIN
	path.remove(QRegExp("[/\\\\]*$"));
#else
	path.remove(QRegExp("/*$"));
#endif

	int lastSlash = path.lastIndexOf('/');
	if (-1 == lastSlash)
#ifdef Q_WS_WIN
		if (-1 == (lastSlash = path.lastIndexOf('\\')))
#endif
			return QString();

	return path.mid(lastSlash + 1);
}

void ThemeManager::loadThemes(QStringList pathList)
{
	pathList = pathList + defaultThemePaths();

	QString currentThemeName = currentTheme().name();
	CurrentThemeIndex = -1;

	Themes.clear();

	// Use " " instead of "" to prevent autoloading of default theme on startup
	if (IncludeNone)
		Themes.append(Theme(" ", tr("None")));

	foreach (const QString &path, pathList)
	{
		if (!isValidThemePath(path))
			continue;

		QString newThemeName = getThemeName(path);
		Theme theme(path + '/', newThemeName);
		Themes.append(theme);

		if (newThemeName == currentThemeName)
			CurrentThemeIndex = Themes.size() - 1;
	}

	if (-1 == CurrentThemeIndex)
		CurrentThemeIndex = getDefaultThemeIndex();

	emit themeListUpdated();
}

void ThemeManager::setCurrentTheme(const QString &themePath)
{
	for (int i = 0; i < Themes.size(); i++)
	{
		const Theme &theme = Themes.at(i);

		if (themePath == theme.name() || themePath == theme.path())
		{
			CurrentThemeIndex = i;
			return;
		}
	}

	CurrentThemeIndex = getDefaultThemeIndex();
}

int ThemeManager::currentThemeIndex() const
{
	return CurrentThemeIndex;
}

const Theme & ThemeManager::currentTheme() const
{
	if (CurrentThemeIndex < 0 || CurrentThemeIndex >= Themes.size())
		return Theme::null;

	return Themes.at(CurrentThemeIndex);
}
