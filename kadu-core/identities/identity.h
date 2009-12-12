/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDENTITY_H
#define IDENTITY_H

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include "identities/identity-shared.h"
#include "exports.h"

#include "storage/shared-base.h"

class Account;

class KADUAPI Identity : public SharedBase<IdentityShared>
{
	KaduSharedBaseClass(Identity)

public:
	static Identity create();
	static Identity loadFromStorage(StoragePoint *identityStoragePoint);
	static Identity null;

	Identity();
	Identity(IdentityShared *data);
	Identity(QObject *data);
	Identity(const Identity &copy);
	virtual ~Identity();

	void addAccount(Account account);
	void removeAccount(Account account);
	bool hasAccount(Account account);

	KaduSharedBase_Property(QString, name, Name)
	KaduSharedBase_Property(QList<Account>, accounts, Accounts)

};

#endif // IDENTITY_H
