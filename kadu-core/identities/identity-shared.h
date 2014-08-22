/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "status/storable-status-container.h"
#include "storage/shared.h"

class Account;

class KADUAPI IdentityShared : public StorableStatusContainer, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(IdentityShared)

	bool Permanent;
	QString Name;
	QList<Account> Accounts;
	Status LastSetStatus;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	static IdentityShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);
	static IdentityShared * loadFromStorage(const std::shared_ptr<StoragePoint> &accountStoragePoint);

	explicit IdentityShared(const QUuid &uuid = QUuid());
	virtual ~IdentityShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	void addAccount(const Account &account);
	void removeAccount(const Account &account);
	bool hasAccount(const Account &account);
	bool hasAnyAccountWithDetails();
	bool isEmpty();

	KaduShared_Property(const QString &, name, Name)
	KaduShared_PropertyBool(Permanent)

	// StatusContainer implementation

	virtual QString statusContainerName()  { return name(); }

	virtual Status status();
	virtual void setStatus(Status status, StatusChangeSource source);
	virtual bool isStatusSettingInProgress();
	virtual int maxDescriptionLength();

	virtual KaduIcon statusIcon();
	virtual KaduIcon statusIcon(const Status &status);

	virtual QList<StatusType> supportedStatusTypes();

};

#endif // IDENTITY_SHARED_H
