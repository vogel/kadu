/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef IDENTITY_SHARED_H
#define IDENTITY_SHARED_H

#include "status/base-status-container.h"
#include "storage/shared.h"

class Account;

class KADUAPI IdentityShared : public BaseStatusContainer, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(IdentityShared)

	bool Permanent;
	QString Name;
	QList<Account> Accounts;

	Account bestAccount();

protected:
	virtual void load();

	virtual void doSetStatus(Status newStatus);

public:
	static IdentityShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint);
	static IdentityShared * loadFromStorage(const QSharedPointer<StoragePoint> &accountStoragePoint);

	explicit IdentityShared(const QUuid &uuid = QUuid());
	virtual ~IdentityShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual void aboutToBeRemoved();

	void addAccount(Account account);
	void removeAccount(Account account);
	bool hasAccount(Account account);
	bool hasAnyAccountWithDetails();
	bool isEmpty();

	KaduShared_Property(QString, name, Name)
	KaduShared_PropertyBool(Permanent)

	// StatusContainer implementation

	virtual QString statusContainerName()  { return name(); }

	virtual Status status();
	virtual int maxDescriptionLength();

	virtual QString statusDisplayName();
	virtual QIcon statusIcon();
    virtual QString statusIconPath(const QString &statusType);
	virtual QIcon statusIcon(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QIcon statusIcon(Status status);
};

#endif // IDENTITY_SHARED_H
