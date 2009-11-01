/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "buddies/buddy.h"

#include "protocol-menu-manager.h"

ProtocolMenuManager::ProtocolMenuManager()
{
}

QList<ActionDescription *> ProtocolMenuManager::protocolActions(Account account, Contact contact)
{
	if (!contact.hasAccountData(account))
		return QList<ActionDescription *>();

	return ProtocolActions;
}
