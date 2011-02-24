/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "account-manager.h"

#include "accounts-aware-object.h"

KADU_AWARE_CLASS(AccountsAwareObject)

void AccountsAwareObject::accountAdded(Account account)
{
	Q_UNUSED(account)
}

void AccountsAwareObject::accountRemoved(Account account)
{
	Q_UNUSED(account)
}

void AccountsAwareObject::notifyAccountAdded(Account account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountAdded(account);
}

void AccountsAwareObject::notifyAccountRemoved(Account account)
{
	foreach (AccountsAwareObject *object, Objects)
		object->accountRemoved(account);
}

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

void AccountsAwareObject::triggerAllAccountsAdded()
{
	foreach (const Account &account, AccountManager::instance()->items())
		accountAdded(account);
}

void AccountsAwareObject::triggerAllAccountsRemoved()
{
	foreach (const Account &account, AccountManager::instance()->items())
		accountRemoved(account);
}

void AccountsAwareObject::triggerAllAccountsRegistered()
{
	foreach (const Account &account, AccountManager::instance()->items())
		accountRegistered(account);
}

void AccountsAwareObject::triggerAllAccountsUnregistered()
{
	foreach (const Account &account, AccountManager::instance()->items())
		accountUnregistered(account);
}
