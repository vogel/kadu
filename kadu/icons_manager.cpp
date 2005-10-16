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

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "toolbar.h"
#include "userbox.h"

IconsManager::IconsManager(const QString& name, const QString& configname)
	:Themes(name, configname, "icons_manager")
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
//	kdebugf();
	QMap<QString, QPixmap>::const_iterator i = icons.find(name);
	if (i != icons.end())
	{
//		kdebugf2();
		return *i;
	}

	QPixmap p;
	if (!p.load(iconPath(name)))
		kdebugmf(KDEBUG_WARNING, "warning - pixmap '%s' cannot be loaded!\n", name.local8Bit().data());
	icons.insert(name, p);
//	kdebugf2();
	return icons[name];
}

const QMovie & IconsManager::loadAnimatedIcon(const QString &name)
{
//	kdebugf();
	QMap<QString, QMovie>::const_iterator i = animatedIcons.find(name);
	if (i != animatedIcons.end())
	{
//		kdebugf2();
		return *i;
	}

	animatedIcons.insert(name, QMovie(iconPath(name)));
//	kdebugf2();
	return animatedIcons[name];
}

void IconsManager::clear()
{
	kdebugf();
	icons.clear();
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
					bool checked = menu->isItemChecked(id);
					menu->changeItem(id, loadIcon((*it2).second), t);
					menu->setItemEnabled(id, enabled);
					menu->setItemChecked(id, checked);
				}
		}
	}
	kdebugf2();
}

void IconsManager::onApplyTabLook()
{
	kdebugf();
	QString previousIconTheme = config_file.readEntry("Look", "IconTheme");
	QComboBox *iconThemeCombo = ConfigDialog::getComboBox("Look", "Icon theme");
	QString selectedTheme;
	if (iconThemeCombo->currentText() == tr("Default"))
		selectedTheme = "default";
	else
	    selectedTheme = iconThemeCombo->currentText();

	if (selectedTheme != previousIconTheme)
	{
		config_file.writeEntry("Look", "IconTheme", selectedTheme);

		icons_manager->clear();
		icons_manager->setTheme(selectedTheme);
		MainToolBar::refreshIcons();
		UserBox::userboxmenu->refreshIcons();
		icons_manager->refreshMenus();
		kadu->changeAppearance();
		QMessageBox::information(0, tr("Icons"), tr("Please close all (except main) Kadu windows"));
	}

	config_file.writeEntry("Look", "IconsPaths", icons_manager->additionalPaths().join(";"));
	kdebugf2();
}

void IconsManager::onCreateTabLook()
{
	kdebugf();

	QComboBox *iconThemeCombo = ConfigDialog::getComboBox("Look", "Icon theme");
	iconThemeCombo->insertStringList(icons_manager->themes());
	iconThemeCombo->setCurrentText(config_file.readEntry("Look", "IconTheme"));

	const QStringList &Themes = icons_manager->themes();
	if (Themes.contains("default"))
		iconThemeCombo->changeItem(tr("Default"), Themes.findIndex("default"));

	QStringList pl(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));
	ConfigDialog::getSelectPaths("Look", "Icon paths")->setPathList(pl);

	kdebugf2();
}

void IconsManager::initModule()
{
	kdebugf();
	icons_manager = new IconsManager ("icons", "icons.conf");
	config_file.addVariable("Look", "IconsPaths", QString::null);
	config_file.addVariable("Look", "IconTheme", "default");

	icons_manager->setPaths(QStringList::split(";", config_file.readEntry("Look", "IconsPaths")));

	QStringList themes = icons_manager->themes();
	if (!themes.contains(config_file.readEntry("Look","IconTheme")) && !themes.isEmpty())
		config_file.writeEntry("Look", "IconTheme", themes[0]);

	icons_manager->setTheme(config_file.readEntry("Look","IconTheme"));

	ConfigDialog::addTab("General", "GeneralTab");
	ConfigDialog::addTab("ShortCuts", "ShortCutsTab");
	ConfigDialog::addTab("Chat", "ChatTab");
	ConfigDialog::addTab("Look", "LookTab");
	ConfigDialog::addHBox("Look", "Look", "icon_theme");
	ConfigDialog::addComboBox("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon theme"));
	ConfigDialog::addSelectPaths("Look", "icon_theme", QT_TRANSLATE_NOOP("@default","Icon paths"));

	ConfigDialog::registerSlotOnCreateTab("Look", icons_manager, SLOT(onCreateTabLook()));
	ConfigDialog::registerSlotOnApplyTab("Look", icons_manager, SLOT(onApplyTabLook()));
	ConfigDialog::connectSlot("Look", "Icon paths", SIGNAL(changed(const QStringList&)),
								icons_manager, SLOT(selectedPaths(const QStringList&)));
	kdebugf2();
}

void IconsManager::selectedPaths(const QStringList& paths)
{
	kdebugf();
	setPaths(paths);
	QComboBox* iconThemeCombo = ConfigDialog::getComboBox("Look","Icon theme");
	QString current = iconThemeCombo->currentText();

	ConfigDialog::getSelectPaths("Look","Icon paths")->setPathList(additionalPaths());

	iconThemeCombo->clear();
	iconThemeCombo->insertStringList(themes());
	iconThemeCombo->setCurrentText(current);

	if (paths.contains("default"))
		iconThemeCombo->changeItem(tr("Default"), paths.findIndex("default"));
	kdebugf2();
}

IconsManager *icons_manager;
