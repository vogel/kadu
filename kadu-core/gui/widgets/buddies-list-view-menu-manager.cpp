/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "contacts/contact-set.h"
#include "gui/actions/action.h"
#include "gui/actions/action-data-source.h"
#include "gui/actions/action-description.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"

#include "buddies-list-view-menu-manager.h"

BuddiesListViewMenuManager * BuddiesListViewMenuManager::Instance = 0;

BuddiesListViewMenuManager * BuddiesListViewMenuManager::instance()
{
	if (!Instance)
		Instance = new BuddiesListViewMenuManager();

	return Instance;
}

BuddiesListViewMenuManager::BuddiesListViewMenuManager() :
		BuddiesContexMenuSorted(true), BuddyListActionsSorted(true)
{
}

void BuddiesListViewMenuManager::sortBuddiesContexMenu()
{
	if (!BuddiesContexMenuSorted)
	{
		qSort(BuddiesContexMenu);
		BuddiesContexMenuSorted = true;
	}
}

void BuddiesListViewMenuManager::sortBuddyListActions()
{
	if (!BuddyListActionsSorted)
	{
		qSort(BuddyListActions);
		BuddyListActionsSorted = true;
	}
}

void BuddiesListViewMenuManager::addActionDescription(ActionDescription *actionDescription, TalkableMenuItem::MenuCategory category, int priority)
{
	BuddiesContexMenu.append(TalkableMenuItem(actionDescription, category, priority));
	BuddiesContexMenuSorted = false;
}

void BuddiesListViewMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	QList<TalkableMenuItem>::iterator i = BuddiesContexMenu.begin();

	while (i != BuddiesContexMenu.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = BuddiesContexMenu.erase(i);
		else
			++i;
	}
}

void BuddiesListViewMenuManager::addListActionDescription(ActionDescription *actionDescription, TalkableMenuItem::MenuCategory category, int priority)
{
	BuddyListActions.append(TalkableMenuItem(actionDescription, category, priority));
	BuddyListActionsSorted = false;
}

void BuddiesListViewMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	QList<TalkableMenuItem>::iterator i = BuddyListActions.begin();

	while (i != BuddyListActions.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = BuddyListActions.erase(i);
		else
			++i;
	}
}

// TODO: refactor, split
QMenu * BuddiesListViewMenuManager::menu(QWidget *parent, ActionDataSource *actionDataSource)
{
	sortBuddiesContexMenu();
	sortBuddyListActions();

	QMenu *menu = new QMenu(parent);

	QMenu *actions = new QMenu(tr("More Actions..."), menu);

	TalkableMenuItem::MenuCategory lastCategory = TalkableMenuItem::CategoryChat;
	bool first = true;
	foreach (TalkableMenuItem menuItem, BuddyListActions)
	{
		if (!first && lastCategory != menuItem.category())
			actions->addSeparator();

		Action *action = menuItem.actionDescription()->createAction(actionDataSource, parent);
		actions->addAction(action);
		action->checkState();

		lastCategory = menuItem.category();
		first = false;
	}

	if (actionDataSource->roles().contains(ContactRole))
	{
		if (1 == actionDataSource->contacts().size())
		{
			Contact contact = *actionDataSource->contacts().constBegin();

			if (contact.contactAccount() && contact.contactAccount().protocolHandler())
			{
				Account account = contact.contactAccount();
				ProtocolFactory *protocolFactory = account.protocolHandler()->protocolFactory();

				if (protocolFactory && protocolFactory->protocolMenuManager())
				{
					if (!first && !protocolFactory->protocolMenuManager()->protocolActions().isEmpty())
						actions->addSeparator();

					foreach (ActionDescription *actionDescription, protocolFactory->protocolMenuManager()->protocolActions())
						if (actionDescription)
						{
							Action *action = actionDescription->createAction(actionDataSource, parent);
							actions->addAction(action);
							action->checkState();
						}
						else
							actions->addSeparator();
				}
			}
		}
	}

	lastCategory = TalkableMenuItem::CategoryChat;
	first = true;
	foreach (TalkableMenuItem menuItem, BuddiesContexMenu)
	{
		if (!first && lastCategory != menuItem.category())
		{
			if (menuItem.category() > TalkableMenuItem::CategoryActions)
				menu->addMenu(actions);
			menu->addSeparator();
		}

		Action *action = menuItem.actionDescription()->createAction(actionDataSource, parent);
		menu->addAction(action);
		action->checkState();

		lastCategory = menuItem.category();
		first = false;
	}

	return menu;
}
