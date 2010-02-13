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

#include "debug.h"
#include "misc/misc.h"

#include "icons-manager.h"

IconsManager * IconsManager::Instance = 0;

IconsManager * IconsManager::instance()
{
	if (Instance == 0)
		Instance = new IconsManager();
	return Instance;
}

IconsManager::IconsManager()
	: Themes("icons", "icons.conf"), pixmaps(), icons()
{
	kdebugf();

	setPaths(config_file.readEntry("Look", "IconsPaths").split(QRegExp("(;|:)"), QString::SkipEmptyParts));
	
	QStringList themeList = themes();
	QString theme = config_file.readEntry("Look", "IconTheme");
	if (!themeList.isEmpty() && !themeList.contains(theme))
	{
		theme = "default";
		config_file.writeEntry("Look", "IconTheme", "default");
	}
	
	setTheme(theme);

	kdebugf2();
}

QString IconsManager::iconPath(const QString &name) const
{
	if (name.startsWith('/'))
		return name;

	QString fname = themePath() + name;
	QString absoluteName = dataPath() + fname;

	if (!QFile::exists(fname) && QFile::exists(absoluteName))
		fname = absoluteName;

	QDir dir(fname);
	if (dir.exists()) // hmm, icon != dir
		return QString::null;

	return fname;
}

const QPixmap &IconsManager::pixmapByPath(const QString &name)
{
	QMap<QString, QPixmap>::const_iterator i = pixmaps.find(name);
	if (i != pixmaps.end())
		return *i;

	QPixmap pix;
	QString path = iconPath(name);
	if (!path.isEmpty())
		pix.load(path);

	pixmaps.insert(name, pix);
	return pixmaps[name];
}

const QIcon &IconsManager::iconByPath(const QString &name)
{
	QMap<QString, QIcon>::const_iterator i = icons.find(name);
	if (i != icons.end())
		return *i;

	QIcon icon;
	QString path = iconPath(name);
	if (!path.isEmpty())
		icon.addFile(path);

	icons.insert(name, icon);
	return icons[name];
}

void IconsManager::clear()
{
	kdebugf();

	pixmaps.clear();
	icons.clear();

	kdebugf2();
}

// TODO: clear it!
void IconsManager::configurationUpdated()
{
	kdebugf();

	bool themeWasChanged = config_file.readEntry("Look", "IconTheme") != IconsManager::instance()->theme();

	clear();
	setTheme(config_file.readEntry("Look", "IconTheme"));
// 	kadu->changeAppearance(); TODO: 0.6.6

	// TODO: Make it standard
	if (themeWasChanged)
	{
		
		emit themeChanged();
	}

	kdebugf2();
}

QSize IconsManager::getIconsSize()
{
	QPixmap p = pixmapByPath("16x16/preferences-other.png");
	return p.size();
}
