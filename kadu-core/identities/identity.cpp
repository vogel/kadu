/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "identity.h"

KaduSharedBaseClassImpl(Identity)

Identity Identity::null;

Identity Identity::create()
{
	return new IdentityShared();
}

Identity Identity::loadStubFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint)
{
	return IdentityShared::loadStubFromStorage(accountStoragePoint);
}

Identity Identity::loadFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint)
{
	return IdentityShared::loadFromStorage(accountStoragePoint);
}

Identity::Identity()
{
}

Identity::Identity(IdentityShared *data) :
		SharedBase<IdentityShared>(data)
{
}

Identity::Identity(QObject *data)
{
	IdentityShared *shared = dynamic_cast<IdentityShared *>(data);
	if (shared)
		setData(shared);
}

Identity::Identity(const Identity &copy) :
		SharedBase<IdentityShared>(copy)
{
}

Identity::~Identity()
{
}

void Identity::addAccount(Account account)
{
	if (!isNull())
		data()->addAccount(account);
}

void Identity::removeAccount(Account account)
{
	if (!isNull())
		data()->removeAccount(account);
}

bool Identity::hasAccount(Account account) const
{
	return !isNull()
			? data()->hasAccount(account)
			: false;
}

bool Identity::hasAnyAccountWithDetails() const
{
	return !isNull()
			? data()->hasAnyAccountWithDetails()
			: false;
}

bool Identity::isEmpty() const
{
	return !isNull()
			? data()->isEmpty()
			: true;
}

KaduSharedBase_PropertyBoolDef(Identity, Permanent, false)
KaduSharedBase_PropertyDef(Identity, QString, name, Name, QString())

