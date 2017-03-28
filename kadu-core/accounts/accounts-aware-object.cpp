/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

QList<AccountsAwareObject *> AccountsAwareObject::Objects;

void AccountsAwareObject::accountAdded(Account account)
{
    Q_UNUSED(account)
}

void AccountsAwareObject::accountRemoved(Account account)
{
    Q_UNUSED(account)
}

AccountsAwareObject::AccountsAwareObject()
{
    Objects.append(this);
}

AccountsAwareObject::~AccountsAwareObject()
{
    Objects.removeAll(this);
}

void AccountsAwareObject::notifyAccountAdded(Account account)
{
    for (AccountsAwareObject *object : Objects)
        object->accountAdded(account);
}

void AccountsAwareObject::notifyAccountRemoved(Account account)
{
    for (AccountsAwareObject *object : Objects)
        object->accountRemoved(account);
}

void AccountsAwareObject::triggerAllAccountsAdded(AccountManager *accountManager)
{
    for (auto const &account : accountManager->items())
        accountAdded(account);
}

void AccountsAwareObject::triggerAllAccountsRemoved(AccountManager *accountManager)
{
    for (auto const &account : accountManager->items())
        accountRemoved(account);
}
