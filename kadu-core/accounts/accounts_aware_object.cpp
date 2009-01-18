/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account_manager.h"

#include "accounts_aware_object.h"

QList<AccountsAwareObject *> AccountsAwareObject::Objects;

void AccountsAwareObject::registerObject(AccountsAwareObject *object)
{
	Objects.append(object);
}

void AccountsAwareObject::unregisterObject(AccountsAwareObject *object)
{
	Objects.remove(object);
}

AccountsAwareObject::AccountsAwareObject()
{
	registerObject(this);
}

AccountsAwareObject::~AccountsAwareObject()
{
	unregisterObject(this);
}

void AccountsAwareObject::notifyAccountRegistered(Account *account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountRegistered(account);
}

void AccountsAwareObject::notifyAccountUnregistered(Account *account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountUnregistered(account);
}

void AccountsAwareObject::triggerAllAccountsRegistered()
{
	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);
}

void AccountsAwareObject::triggerAllAccountsUnregistered()
{
	foreach (Account *account, AccountManager::instance()->accounts())
		accountUnregistered(account);
}
