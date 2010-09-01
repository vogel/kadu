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

QString IconsManager::iconPath(const QString &path) const
{
	if (path.startsWith('/'))
		return path;

	QString fileName = ThemeManager->currentTheme().path() + path;

	QDir dir(fileName);
	if (dir.exists()) // hmm, icon != dir
		return QString::null;

	return fileName;
}

const QPixmap & IconsManager::pixmapByPath(const QString &path)
{
	if (!PixmapCache.contains(path))
	{
		QPixmap pix;
		QString fullPath = iconPath(path);
		if (!fullPath.isEmpty())
			pix.load(fullPath);
		PixmapCache.insert(path, pix);
	}

	return PixmapCache[path];
}

const QIcon & IconsManager::iconByPath(const QString &path)
{
	if (!IconCache.contains(path))
	{
		QIcon icon;
		QString fullPath = iconPath(path);
		if (!fullPath.isEmpty())
			icon.addFile(fullPath);
		IconCache.insert(path, icon);
	}

	return IconCache[path];
}

void IconsManager::clearCache()
{
	kdebugf();

	PixmapCache.clear();
	IconCache.clear();

	kdebugf2();
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
	QPixmap p = pixmapByPath("16x16/preferences-other.png");
	return p.size();
}
