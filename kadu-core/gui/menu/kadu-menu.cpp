/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QTimer>

#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "menu-item.h"

#include "kadu-menu.h"

KaduMenu::KaduMenu(MenuCategory category, KaduMenu *parent) :
		QObject(parent), Category(category), IsSorted(true)
{
}

KaduMenu * KaduMenu::addAction(ActionDescription *actionDescription, KaduMenu::MenuSection section, int priority)
{
	Items.append(new MenuItem(actionDescription, section, priority));
	IsSorted = false;

	return this;
}

KaduMenu * KaduMenu::addMenu(QMenu *menu, KaduMenu::MenuSection section, int priority)
{
	MenuItem *item = new MenuItem(menu, section, priority);
	Items.append(item);
	IsSorted = false;

	return this;
}

void KaduMenu::removeAction(ActionDescription *actionDescription)
{
	QList<MenuItem*>::iterator i = Items.begin();

	while (i != Items.end())
	{
		if ((*i)->actionDescription() == actionDescription)
			i = Items.erase(i);
		else
			++i;
	}
}

bool KaduMenu::lessThan(const MenuItem *a, const MenuItem *b)
{
	if (a->section() == b->section())
	{
		return a->priority() < b->priority();
	}

	return a->section() < b->section();
}

void KaduMenu::sort()
{
	if (!IsSorted)
	{
		qSort(Items.begin(), Items.end(), lessThan);
		IsSorted = true;
	}
}

void KaduMenu::setGuiMenu ( QMenu* menu )
{
	GuiMenu = menu;
}

void KaduMenu::updateGuiMenu()
{
	sort();

	GuiMenu->clear();

	ActionContext *actionContext = getActionContext();

	bool firstItem = true;
	MenuSection latestSection;

	foreach (MenuItem* menuItem, Items)
	{
		if (!firstItem && latestSection != menuItem->section())
		{
// 			if (menuItem.category() > TalkableMenuItem::CategoryActions)
// 				menu->addMenu(actions);
			GuiMenu->addSeparator();
		}

		if (menuItem->actionDescription())
		{
			Action *action = menuItem->actionDescription()->createAction(actionContext, GuiMenu->parentWidget());
			GuiMenu->addAction(action);
			action->checkState();
		}
		else if (menuItem->submenu())
		{
			GuiMenu->addMenu(menuItem->submenu());
		}

		latestSection = menuItem->section();
		firstItem = false;
	}
}

void KaduMenu::updateGuiMenuLater()
{
	QTimer::singleShot(1000, this, SLOT(updateGuiMenu()));
}

ActionContext * KaduMenu::getActionContext()
{
	return Core::instance()->kaduWindow()->actionContext();
}

void KaduMenu::update()
{
	updateGuiMenuLater();
}
