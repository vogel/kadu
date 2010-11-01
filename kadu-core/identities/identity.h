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
	static Identity loadStubFromStorage(const QSharedPointer<StoragePoint> &identityStoragePoint);
	static Identity loadFromStorage(const QSharedPointer<StoragePoint> &identityStoragePoint);
	static Identity null;

	Identity();
	Identity(IdentityShared *data);
	Identity(QObject *data);
	Identity(const Identity &copy);
	virtual ~Identity();

	void addAccount(Account account);
	void removeAccount(Account account);
	bool hasAccount(Account account);
	bool isEmpty();

	KaduSharedBase_PropertyBool(Permanent)
	KaduSharedBase_Property(QString, name, Name)

};

Q_DECLARE_METATYPE(Identity)

#endif // IDENTITY_H
