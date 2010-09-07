/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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
#include <QtCore/QFile>

#include "configuration/configuration-file.h"
#include "misc/misc.h"
#include "themes/icon-theme-manager.h"
#include "debug.h"

#include "icons-manager.h"

IconsManager * IconsManager::Instance = 0;

IconsManager * IconsManager::instance()
{
	if (Instance == 0)
		Instance = new IconsManager();
	return Instance;
}

IconsManager::IconsManager()
{
	kdebugf();

	QStringList iconPaths = config_file.readEntry("Look", "IconsPaths").split(QRegExp("(;|:)"), QString::SkipEmptyParts);

	ThemeManager = new IconThemeManager(this);
	ThemeManager->loadThemes(iconPaths);
	ThemeManager->setCurrentTheme(config_file.readEntry("Look", "IconTheme"));
	configurationUpdated();

	config_file.writeEntry("Look", "IconTheme", ThemeManager->currentTheme().path());

	kdebugf2();
}

IconThemeManager * IconsManager::themeManager() const
{
	return ThemeManager;
}

QString IconsManager::iconPath(const QString &path, const QString &size, const QString &name) const
{
	QString fileName = ThemeManager->currentTheme().path() + path + "/" + size + "/" + name;

	QDir dir(fileName);
	if (dir.exists()) // hmm, icon != dir
		return QString::null;

	return dir.canonicalPath();
}

QString IconsManager::iconPath(const QString &path, const QString &size) const
{
	QString realPath;
	QString iconName;

	int lastHash = path.lastIndexOf('/');
	if (-1 != lastHash)
	{
		realPath = path.mid(0, lastHash);
		iconName = path.mid(lastHash + 1);
	}
	else
		iconName = path;

	return iconPath(realPath, size, iconName);
}

QString IconsManager::iconPath(const QString &path) const
{
	QString fileName = ThemeManager->currentTheme().path() + path;

	QDir dir(fileName);
	if (dir.exists()) // hmm, icon != dir
		return QString::null;

	return dir.canonicalPath();  
}

QIcon IconsManager::buildIcon(const QString &path)
{
	static QLatin1String sizes [] = {
		QLatin1String("16x16"),
		QLatin1String("22x22"),
		QLatin1String("32x32"),
		QLatin1String("64x64"),
		QLatin1String("128x128")
	};
	static int sizes_count = 5;

	QIcon icon;
	for (int i = 0; i < sizes_count; i++)
	{
		QString fullPath = iconPath(path, sizes[i]);
		if (!fullPath.isEmpty())
			icon.addFile(fullPath);
	}

	return icon;
}

const QIcon & IconsManager::iconByPath(const QString &path)
{
	if (!IconCache.contains(path))
	{
		QIcon icon = buildIcon(path);
		IconCache.insert(path, icon);
	}

	return IconCache[path];
}

void IconsManager::clearCache()
{
	IconCache.clear();
}

void IconsManager::configurationUpdated()
{
	kdebugf();

	bool themeWasChanged = config_file.readEntry("Look", "IconTheme") != ThemeManager->currentTheme().path();
	if (themeWasChanged)
	{
		clearCache();
		ThemeManager->setCurrentTheme(config_file.readEntry("Look", "IconTheme"));
		config_file.writeEntry("Look", "IconTheme", ThemeManager->currentTheme().path());

		emit themeChanged();
	}

	kdebugf2();
}

QSize IconsManager::getIconsSize()
{
	return QSize(16, 16);
}
