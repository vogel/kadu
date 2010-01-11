/*
 * %kadu copyright begin%
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
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

#include "accounts/account.h"
#include "status/base-status-container.h"
#include "storage/shared.h"

class KADUAPI IdentityShared : public BaseStatusContainer, public Shared
{
	Q_DISABLE_COPY(IdentityShared)

	QString Name;
	QList<Account> Accounts;

protected:
	virtual void load();

public:
	static IdentityShared * loadFromStorage(StoragePoint *accountStoragePoint);

	explicit IdentityShared(const QUuid &uuid = QUuid());
	virtual ~IdentityShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual void aboutToBeRemoved();

	void addAccount(Account account);
	void removeAccount(Account account);
	bool hasAccount(Account account);

	KaduShared_Property(QString, name, Name)
	KaduShared_Property(QList<Account>, accounts, Accounts)

	// StatusContainer implementation

	virtual QString statusContainerName()  { return name(); };

	virtual void setStatus(Status newStatus);
	virtual Status status();
	virtual int maxDescriptionLength();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	QPixmap statusPixmap(Status status);

	virtual void setPrivateStatus(bool isPrivate);

};

#endif // IDENTITY_SHARED_H
