/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LIST_VIEW_MENU_MANAGER_H
#define CONTACTS_LIST_VIEW_MENU_MANAGER_H

#include <QtCore/QList>

#include "exports.h"

class ActionDescription;

class KADUAPI ContactsListViewMenuManager
{
	Q_DISABLE_COPY(ContactsListViewMenuManager)

static ContactsListViewMenuManager *Instance;

	QList<ActionDescription *> ContactsContexMenu;
	QList<ActionDescription *> ContactsListActions;
	QList<ActionDescription *> ManagementActions;

	ContactsListViewMenuManager();

public:

	static ContactsListViewMenuManager * instance();

	QList<ActionDescription *> contactsContexMenu() { return ContactsContexMenu; }
	QList<ActionDescription *> contactsListActions() { return ContactsListActions; }
	QList<ActionDescription *> managementActions() { return ManagementActions; }

	void addActionDescription(ActionDescription *actionDescription);
	void insertActionDescription(int pos, ActionDescription *actionDescription);
	void removeActionDescription(ActionDescription *actionDescription);
	void addSeparator();
	void insertSeparator(int pos);
	void addListActionDescription(ActionDescription *actionDescription);
	void insertListActionDescription(int pos, ActionDescription *actionDescription);
	void removeListActionDescription(ActionDescription *actionDescription);
	void addListSeparator();
	void addManagementActionDescription(ActionDescription *actionDescription);
	void insertManagementActionDescription(int pos, ActionDescription *actionDescription);
	void removeManagementActionDescription(ActionDescription *actionDescription);
	void addManagementSeparator();

};

#endif // CONTACTS_LIST_VIEW_MENU_MANAGER_H
