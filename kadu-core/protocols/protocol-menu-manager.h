/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROTOCOL_MENU_MANAGER_H
#define PROTOCOL_MENU_MANAGER_H

#include <QtCore/QList>

class Account;
class ActionDescription;
class Buddy;

class ProtocolMenuManager
{
	// TODO : remove or use it somewhere or extend to kadu menu
	QList<ActionDescription *> ContactsListActions;
	QList<ActionDescription *> ManagementActions;
	QList<ActionDescription *> ProtocolActions;

	ProtocolMenuManager();

public:
	QList<ActionDescription *> contactsListActions() { return ContactsListActions; }
	QList<ActionDescription *> managementActions() { return ManagementActions; }
	QList<ActionDescription *> protocolActions(Account account, Buddy buddy);
};

#endif // PROTOCOL_MENU_MANAGER_H
