/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QPixmap>

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "identity.h"

Identity::Identity(IdentityShared *data) :
		Data(data)
{
}

void Identity::addAccount(Account account)
{
	if (accounts().contains(account))
		return;

	if (Data)
		Data->addAccount(account);
}

bool Identity::hasAccount(Account account) const
{
	return accounts().contains(account);
}

void Identity::removeAccount(Account account)
{
	accounts().removeAll(account);
	accountsUuids().removeAll(account.uuid());
}



