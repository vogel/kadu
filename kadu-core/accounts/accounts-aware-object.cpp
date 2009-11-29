/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account-manager.h"

#include "accounts-aware-object.h"

KADU_AWARE_CLASS(AccountsAwareObject)

void AccountsAwareObject::notifyAccountRegistered(Account account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountRegistered(account);
}

void AccountsAwareObject::notifyAccountUnregistered(Account account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountUnregistered(account);
}

void AccountsAwareObject::triggerAllAccountsRegistered()
{
	foreach (Account account, AccountManager::instance()->items())
		accountRegistered(account);
}

void AccountsAwareObject::triggerAllAccountsUnregistered()
{
	foreach (Account account, AccountManager::instance()->items())
		accountUnregistered(account);
}
