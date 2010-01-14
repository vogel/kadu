/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "userbox.h"

#include "icons_manager.h"

IconsManager::IconsManager(const QString &name, const QString &configname)
	: Themes(name, configname), pixmaps(), icons()
{
	kdebugf();
	kdebugf2();
}

QString IconsManager::iconPath(const QString &name) const
{
	QString fname;
	QString absoluteName;
	if(name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);

	absoluteName = dataPath() + fname;
	if(!QFile::exists(fname) && QFile::exists(absoluteName))
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

	bool themeWasChanged = config_file.readEntry("Look", "IconTheme") != icons_manager->theme();

	icons_manager->clear();
	icons_manager->setTheme(config_file.readEntry("Look", "IconTheme"));
	kadu->changeAppearance();

	// TODO: Make it standard
	if (themeWasChanged)
	{
		
		emit themeChanged();
	}

	kdebugf2();
}

void IconsManager::initModule()
{
	kdebugf();

	icons_manager = new IconsManager("icons", "icons.conf");

	icons_manager->setPaths(QStringList::split(QRegExp("(;|&)"), config_file.readEntry("Look", "IconsPaths")));

	QStringList themes = icons_manager->themes();
	QString theme = config_file.readEntry("Look", "IconTheme");
	if (!themes.isEmpty() && !themes.contains(theme))
	{
		theme = "default";
		config_file.writeEntry("Look", "IconTheme", "default");
	}

	icons_manager->setTheme(theme);

	kdebugf2();
}

void IconsManager::closeModule()
{
	kdebugf();

	delete icons_manager;
	icons_manager = 0;

	kdebugf2();
}

QSize IconsManager::getIconsSize()
{
	QPixmap p = loadPixmap("Configuration");
	return p.size();
}

IconsManager *icons_manager;
