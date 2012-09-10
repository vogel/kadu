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

#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/menu/menu-inventory.h"
#include "menu-item.h"
#include "protocols/protocol-menu-manager.h"

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
	if (IsSorted)
		return;

	qSort(Items.begin(), Items.end(), lessThan);
	IsSorted = true;
}

void KaduMenu::setGuiMenu ( QMenu* menu )
{
	GuiMenu = menu;
}

void KaduMenu::updateGuiMenu(ActionContext *context)
{
	sort();

	GuiMenu->clear();

	ActionContext *actionContext = context
		? context
		: getActionContext();

	bool firstItem = true;
	MenuSection latestSection;

	QMenu *actions = new QMenu(tr("More Actions..."), GuiMenu);

	foreach (MenuItem* menuItem, Items)
	{
		QMenu *currentMenu = menuItem->section() == KaduMenu::SectionActions
			? actions
			: GuiMenu;

		if (!firstItem && latestSection != menuItem->section())
		{
			currentMenu->addSeparator();
		}

		Action *action = menuItem->actionDescription()->createAction(actionContext, currentMenu->parent());
		currentMenu->addAction(action);
		action->checkState();

		latestSection = menuItem->section();
		firstItem = false;
	}

	if (CategoryBuddiesList != Category)
	{
		return;
	}

	if (actionContext->roles().contains(ContactRole) && 1 == actionContext->contacts().size())
	{
		foreach (ProtocolMenuManager *manager, MenuInventory::instance()->protocolMenuManagers())
		{
			Contact contact = *actionContext->contacts().constBegin();
			if (contact.contactAccount().protocolName() != manager->protocolName())
				continue;

			if (!firstItem && !manager->protocolActions().isEmpty())
				actions->addSeparator();

			foreach (ActionDescription *actionDescription, manager->protocolActions())
			{
				if (actionDescription)
				{
					Action *action = actionDescription->createAction(actionContext, GuiMenu->parent());
					actions->addAction(action);
					action->checkState();
				}
				else
					actions->addSeparator();
			}
		}
	}

	if (actions->actions().size() > 0)
		GuiMenu->addMenu(actions);
}

void KaduMenu::updateGuiMenuLater()
{
	QTimer::singleShot(1000, this, SLOT(updateGuiMenuSlot()));
}

void KaduMenu::updateGuiMenuSlot()
{
	updateGuiMenu();
}

QMenu * KaduMenu::menu(QWidget *parent, ActionContext *actionContext)
{
	GuiMenu = new QMenu(parent);
	updateGuiMenu(actionContext);

	return GuiMenu;
}

ActionContext * KaduMenu::getActionContext()
{
	return Core::instance()->kaduWindow()->actionContext();
}

void KaduMenu::update()
{
	updateGuiMenuLater();
}
