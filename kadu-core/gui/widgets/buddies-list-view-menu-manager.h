/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_MENU_MANAGER_H
#define BUDDIES_LIST_VIEW_MENU_MANAGER_H

#include <QtCore/QObject>

#include "gui/widgets/buddies-list-view-menu-item.h"
#include "exports.h"

class QMenu;
class QWidget;

class ActionDataSource;
class ActionDescription;
class Contact;

class KADUAPI BuddiesListViewMenuManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddiesListViewMenuManager)

	static BuddiesListViewMenuManager *Instance;

	QList<BuddiesListViewMenuItem> BuddiesContexMenu;
	bool BuddiesContexMenuSorted;

	QList<BuddiesListViewMenuItem> BuddyListActions;
	bool BuddyListActionsSorted;

	BuddiesListViewMenuManager();

	void sortBuddiesContexMenu();
	void sortBuddyListActions();

public:
	static BuddiesListViewMenuManager * instance();

	void addActionDescription(ActionDescription *actionDescription, BuddiesListViewMenuItem::BuddiesListViewMenuCategory category, int priority);
	void removeActionDescription(ActionDescription *actionDescription);

	void addListActionDescription(ActionDescription *actionDescription, BuddiesListViewMenuItem::BuddiesListViewMenuCategory category, int priority);
	void removeListActionDescription(ActionDescription *actionDescription);

	QMenu * menu(QWidget *parent, ActionDataSource *actionDataSource, const QList<Contact> &contacts);

};

#endif // BUDDIES_LIST_VIEW_MENU_MANAGER_H
