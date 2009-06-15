/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-file.h"

#include "action.h"
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
	QString fname;
	QString absoluteName;
	if (name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);

	absoluteName = dataPath() + fname;
	if (!QFile::exists(fname) && QFile::exists(absoluteName))
		fname = absoluteName;

	return fname;
}

const QPixmap &IconsManager::loadPixmap(const QString &name)
{
	QMap<QString, QPixmap>::const_iterator i = pixmaps.find(name);
	if (i != pixmaps.end())
		return *i;

	QPixmap pix(iconPath(name));

	pixmaps.insert(name, pix);
	return pixmaps[name];
}

const QIcon &IconsManager::loadIcon(const QString &name)
{
	QMap<QString, QIcon>::const_iterator i = icons.find(name);
	if (i != icons.end())
		return *i;

	QIcon icon(iconPath(name));

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
	QPixmap p = loadPixmap("Configuration");
	return p.size();
}
