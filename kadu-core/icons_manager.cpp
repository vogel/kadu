/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcombobox.h>
#include <qmessagebox.h>

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "userbox.h"

IconsManager::IconsManager(const QString& name, const QString& configname) :
	Themes(name, configname, "icons_manager"), icons(), iconSets(), animatedIcons(), menus()
{
	kdebugf();
	kdebugf2();
}

QString IconsManager::iconPath(const QString &name) const
{
	QString fname;
	if(name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);
	return fname;
}

const QPixmap &IconsManager::loadIcon(const QString &name)
{
	QMap<QString, QPixmap>::const_iterator i = icons.find(name);
	if (i != icons.end())
		return *i;

	QPixmap p;
	if (!p.load(iconPath(name)))
		kdebugmf(KDEBUG_WARNING, "warning - pixmap '%s' cannot be loaded!\n", name.local8Bit().data());

	icons.insert(name, p);
	return icons[name];
}

const QIconSet &IconsManager::loadIconSet(const QString &name)
{
	QMap<QString, QIconSet>::const_iterator i = iconSets.find(name);
	if (i != iconSets.end())
		return *i;

	QIconSet set;
	set.setPixmap(iconPath(name), QIconSet::Automatic, QIconSet::Normal);
	set.setPixmap(iconPath(name + "_dis"), QIconSet::Automatic, QIconSet::Disabled);

	iconSets.insert(name, set);
	return iconSets[name];
}

const QMovie &IconsManager::loadAnimatedIcon(const QString &name)
{
	QMap<QString, QMovie>::const_iterator i = animatedIcons.find(name);
	if (i != animatedIcons.end())
		return *i;

	animatedIcons.insert(name, QMovie(iconPath(name)));
	return animatedIcons[name];
}

void IconsManager::clear()
{
	kdebugf();

	icons.clear();
	iconSets.clear();

	kdebugf2();
}

void IconsManager::registerMenu(QMenuData *menu)
{
	kdebugf();
	menus.push_front(qMakePair(menu, QValueList<QPair<QString, QString> >()));
	kdebugf2();
}

void IconsManager::unregisterMenu(QMenuData *menu)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first == menu)
		{
			menus.remove(it);
			break;
		}
	kdebugf2();
}

void IconsManager::registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first == menu)
		{
			(*it).second.push_front(qMakePair(caption, iconName));
			break;
		}
	kdebugf2();
}

void IconsManager::unregisterMenuItem(QMenuData *menu, const QString &caption)
{
	kdebugf();
	FOREACH(it, menus)
		if ((*it).first == menu)
		{
			CONST_FOREACH(it2, (*it).second)
				if ((*it2).first == caption)
				{
					(*it).second.remove(*it2);
					break;
				}
			break;
		}
	kdebugf2();
}

void IconsManager::refreshMenus()
{
	kdebugf();
	CONST_FOREACH(it, menus)
	{
		QMenuData *menu = (*it).first;
		for (unsigned int i = 0, count = menu->count(); i < count; ++i)
		{
			int id = menu->idAt(i);
			QString t = menu->text(id);

			CONST_FOREACH(it2, (*it).second)
				//startsWith jest potrzebne, bo je¿eli opcja w menu ma skrót klawiszowy,
				//to menu->text(id) zwraca napis "Nazwa opcji\tskrót klawiszowy"
				if (t == (*it2).first || t.startsWith((*it2).first + "\t"))
				{
					bool enabled = menu->isItemEnabled(id);
					bool visible = menu->isItemVisible(id);
					bool checked = menu->isItemChecked(id);
					menu->changeItem(id, loadIcon((*it2).second), t);
					menu->setItemEnabled(id, enabled);
					menu->setItemVisible(id, visible);
					menu->setItemChecked(id, checked);
				}
		}
	}
	kdebugf2();
}

// TODO: clear it!
void IconsManager::configurationUpdated()
{
	kdebugf();

	icons_manager->clear();
	icons_manager->setTheme(config_file.readEntry("Look", "IconTheme"));
	KaduActions.refreshIcons();
	UserBox::userboxmenu->refreshIcons();
	UserBox::management->refreshIcons();
	icons_manager->refreshMenus();
	kadu->changeAppearance();

	kdebugf2();
}

void IconsManager::initModule()
{
	kdebugf();

	icons_manager = new IconsManager("icons", "icons.conf");

	icons_manager->setPaths(QStringList::split(QRegExp("(;|:)"), config_file.readEntry("Look", "IconsPaths")));

	QStringList themes = icons_manager->themes();
	QString theme = config_file.readEntry("Look", "IconTheme");
	if (!themes.isEmpty() && !themes.contains(theme))
		config_file.writeEntry("Look", "IconTheme", "default");

//	icons_manager->setTheme(theme);

	kdebugf2();
}

void IconsManager::closeModule()
{
	kdebugf();

	delete icons_manager;
	icons_manager = 0;

	kdebugf2();
}

IconsManager *icons_manager;
