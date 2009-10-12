/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts-list-view-menu-manager.h"

ContactsListViewMenuManager * ContactsListViewMenuManager::Instance = 0;

ContactsListViewMenuManager * ContactsListViewMenuManager::instance()
{
	if (!Instance)
		Instance = new ContactsListViewMenuManager();

	return Instance;
}

ContactsListViewMenuManager::ContactsListViewMenuManager()
{
}

void ContactsListViewMenuManager::addActionDescription(ActionDescription *actionDescription)
{
	ContactsContexMenu.append(actionDescription);
}

void ContactsListViewMenuManager::insertActionDescription(int pos, ActionDescription *actionDescription)
{
	ContactsContexMenu.insert(pos, actionDescription);
}

void ContactsListViewMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	ContactsContexMenu.removeAll(actionDescription);
}

void ContactsListViewMenuManager::addSeparator()
{
	ContactsContexMenu.append(0);
}

void ContactsListViewMenuManager::insertSeparator(int pos)
{
	ContactsContexMenu.insert(pos, 0);
}

void ContactsListViewMenuManager::addListActionDescription(ActionDescription *actionDescription)
{
	ContactsListActions.append(actionDescription);
}

void ContactsListViewMenuManager::insertListActionDescription(int pos, ActionDescription *actionDescription)
{
	ContactsListActions.insert(pos, actionDescription);
}

void ContactsListViewMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	ContactsListActions.removeAll(actionDescription);
}

void ContactsListViewMenuManager::addListSeparator()
{
	ContactsListActions.append(0);
}

void ContactsListViewMenuManager::addManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.append(actionDescription);
}

void ContactsListViewMenuManager::insertManagementActionDescription(int pos, ActionDescription *actionDescription)
{
	ManagementActions.insert(pos, actionDescription);
}

void ContactsListViewMenuManager::removeManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.removeAll(actionDescription);
}

void ContactsListViewMenuManager::addManagementSeparator()
{
	ManagementActions.append(0);
}
