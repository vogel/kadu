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

Identity Identity::null(true);

Identity Identity::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return IdentityShared::loadFromStorage(accountStoragePoint);
}

Identity::Identity(bool null) :
		SharedBase<IdentityShared>(null)
{
}

Identity::Identity()
{
	data()->setState(StorableObject::StateNew);
}

Identity::Identity(IdentityShared *data) :
		SharedBase<IdentityShared>(data)
{
}

Identity::Identity(QObject *data) :
		SharedBase<IdentityShared>(true)
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

bool Identity::hasAccount(Account account)
{
	return !isNull()
			? data()->hasAccount(account)
			: false;
}

KaduSharedBase_PropertyDef(Identity, QString, name, Name, QString::null)
KaduSharedBase_PropertyDef(Identity, QList<Account>, accounts, Accounts, QList<Account>())
