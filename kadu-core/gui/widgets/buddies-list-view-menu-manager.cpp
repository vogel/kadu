/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies-list-view-menu-manager.h"

BuddiesListViewMenuManager * BuddiesListViewMenuManager::Instance = 0;

BuddiesListViewMenuManager * BuddiesListViewMenuManager::instance()
{
	if (!Instance)
		Instance = new BuddiesListViewMenuManager();

	return Instance;
}

BuddiesListViewMenuManager::BuddiesListViewMenuManager()
{
}

void BuddiesListViewMenuManager::addActionDescription(ActionDescription *actionDescription)
{
	BuddiesContexMenu.append(actionDescription);
}

void BuddiesListViewMenuManager::insertActionDescription(int pos, ActionDescription *actionDescription)
{
	BuddiesContexMenu.insert(pos, actionDescription);
}

void BuddiesListViewMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	BuddiesContexMenu.removeAll(actionDescription);
}

void BuddiesListViewMenuManager::addSeparator()
{
	BuddiesContexMenu.append(0);
}

void BuddiesListViewMenuManager::insertSeparator(int pos)
{
	BuddiesContexMenu.insert(pos, 0);
}

void BuddiesListViewMenuManager::addListActionDescription(ActionDescription *actionDescription)
{
	BuddyListActions.append(actionDescription);
}

void BuddiesListViewMenuManager::insertListActionDescription(int pos, ActionDescription *actionDescription)
{
	BuddyListActions.insert(pos, actionDescription);
}

void BuddiesListViewMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	BuddyListActions.removeAll(actionDescription);
}

void BuddiesListViewMenuManager::addListSeparator()
{
	BuddyListActions.append(0);
}

void BuddiesListViewMenuManager::addManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.append(actionDescription);
}

void BuddiesListViewMenuManager::insertManagementActionDescription(int pos, ActionDescription *actionDescription)
{
	ManagementActions.insert(pos, actionDescription);
}

void BuddiesListViewMenuManager::removeManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.removeAll(actionDescription);
}

void BuddiesListViewMenuManager::addManagementSeparator()
{
	ManagementActions.append(0);
}
