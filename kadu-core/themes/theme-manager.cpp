/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "theme-manager.h"

ThemeManager::ThemeManager(QObject *parent) :
		QObject(parent)
{
}

ThemeManager::~ThemeManager()
{
}

QStringList ThemeManager::getSubDirs(const QString &dirPath) const
{
	QDir dir(dirPath);
	QStringList subDirs = dir.entryList(QDir::Dirs);

	QStringList result;
	foreach (const QString &subDir, subDirs)
		if (!subDir.startsWith('.')) // ignore hidden, this, and parent
			result.append(dirPath + '/' + subDir);

	return result;
}

void ThemeManager::loadThemes()
{
	Themes.clear();

	QStringList themePaths = defaultThemePaths();
	foreach (const QString &path, themePaths)
	{
		if (!isValidThemePath(path))
			continue;

		QString name = QDir(path).dirName();
		if (Themes.contains(name))
			continue;

		Theme theme(path + '/', name);
		Themes.insert(name, theme);
	}

	setCurrentTheme(CurrentThemeName);

	emit themeListUpdated();
}

void ThemeManager::setCurrentTheme(const QString &themeName)
{
	// compatibility with pre-0.12 versions
	QString fixedName = themeName;
	// custom themes had two trailing slashes and QDir::dirName() was returning empty string
	fixedName.replace(QRegExp("/*$"), QString());
	if (QFileInfo(fixedName).isAbsolute())
		fixedName = QDir(fixedName).dirName();

	if (Themes.contains(fixedName))
		CurrentThemeName = fixedName;
	else
		CurrentThemeName = defaultThemeName();
}

Theme ThemeManager::currentTheme() const
{
	return Themes.value(CurrentThemeName);
}

#include "moc_theme-manager.cpp"
