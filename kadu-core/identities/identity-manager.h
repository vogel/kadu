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

#include "storage/simple-manager.h"

#include "exports.h"

class Account;
class Identity;
class Status;

class KADUAPI IdentityManager : public QObject, public SimpleManager<Identity>
{
	Q_OBJECT
	Q_DISABLE_COPY(IdentityManager)

	static IdentityManager *Instance;

	IdentityManager();
	virtual ~IdentityManager();

protected:
	virtual void itemAboutToBeAdded(Identity item);
	virtual void itemAdded(Identity item);
	virtual void itemAboutToBeRemoved(Identity item);
	virtual void itemRemoved(Identity item);

public:
	static IdentityManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Identities"); }
	virtual QString storageNodeItemName() { return QLatin1String("Identity"); }

	Identity byName(const QString &name, bool create = true);
	Identity identityForAcccount(Account account);

signals:
	void identityAboutToBeRegistered(Identity);
	void identityRegistered(Identity);
	void identityAboutToBeUnregistered(Identity);
	void identityUnregistered(Identity);

};

#include "identities/identity.h" // for MOC

#endif // IDENTITY_MANAGER_H
