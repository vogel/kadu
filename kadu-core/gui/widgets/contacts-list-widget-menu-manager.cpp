/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts-list-widget-menu-manager.h"

ContactsListWidgetMenuManager * ContactsListWidgetMenuManager::Instance = 0;

ContactsListWidgetMenuManager * ContactsListWidgetMenuManager::instance()
{
	if (!Instance)
		Instance = new ContactsListWidgetMenuManager();

	return Instance;
}

ContactsListWidgetMenuManager::ContactsListWidgetMenuManager()
{
}

void ContactsListWidgetMenuManager::addActionDescription(ActionDescription *actionDescription)
{
	ContactsContexMenu.append(actionDescription);
}

void ContactsListWidgetMenuManager::insertActionDescription(int pos, ActionDescription *actionDescription)
{
	ContactsContexMenu.insert(pos, actionDescription);
}

void ContactsListWidgetMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	ContactsContexMenu.removeAll(actionDescription);
}

void ContactsListWidgetMenuManager::addSeparator()
{
	ContactsContexMenu.append(0);
}

void ContactsListWidgetMenuManager::insertSeparator(int pos)
{
	ContactsContexMenu.insert(pos, 0);
}

void ContactsListWidgetMenuManager::addListActionDescription(ActionDescription *actionDescription)
{
	ContactsListActions.append(actionDescription);
}

void ContactsListWidgetMenuManager::insertListActionDescription(int pos, ActionDescription *actionDescription)
{
	ContactsListActions.insert(pos, actionDescription);
}

void ContactsListWidgetMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	ContactsListActions.removeAll(actionDescription);
}

void ContactsListWidgetMenuManager::addListSeparator()
{
	ContactsListActions.append(0);
}

void ContactsListWidgetMenuManager::addManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.append(actionDescription);
}

void ContactsListWidgetMenuManager::insertManagementActionDescription(int pos, ActionDescription *actionDescription)
{
	ManagementActions.insert(pos, actionDescription);
}

void ContactsListWidgetMenuManager::removeManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.removeAll(actionDescription);
}

void ContactsListWidgetMenuManager::addManagementSeparator()
{
	ManagementActions.append(0);
}
