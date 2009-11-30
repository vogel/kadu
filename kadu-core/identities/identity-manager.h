/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDENTITY_MANAGER_H
#define IDENTITY_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "storage/storable-object.h"

#include "exports.h"

class Account;
class Identity;
class Status;

class KADUAPI IdentityManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(IdentityManager)

	static IdentityManager *Instance;

	IdentityManager();
	virtual ~IdentityManager();

	QList<Identity *> Identities;

protected:
	virtual void load();
	virtual StoragePoint * createStoragePoint();

public:
	static IdentityManager * instance();

	virtual void store();

	Identity * byUuid(const QString &uuid);
	Identity * byName(const QString &name, bool create = true);
	Identity * identityForAcccount(Account account);

	const QList<Identity *> identities() { return Identities; }

	void registerIdentity(Identity *identity);
	void unregisterIdentity(Identity *identity);
	void deleteIdentity(Identity *identity);

signals:
	void identityAboutToBeRegistered(Identity *);
	void identityRegistered(Identity *);
	void identityAboutToBeUnregistered(Identity *);
	void identityUnregistered(Identity *);

};

#endif // IDENTITY_MANAGER_H
