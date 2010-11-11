/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONTACT_SHARED_H
#define CONTACT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "avatars/avatar.h"
#include "buddies/buddy.h"
#include "contacts/contact-details.h"
#include "protocols/protocols-aware-object.h"
#include "status/status.h"
#include "storage/details-holder.h"
#include "storage/shared.h"

class ContactManager;

class KADUAPI ContactShared : public QObject, public Shared, public DetailsHolder<ContactShared, ContactDetails, ContactManager>, ProtocolsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactShared)

	Account ContactAccount;
	Avatar ContactAvatar;
	Buddy OwnerBuddy;
	QString Id;
	int Priority;
	short int MaximumImageSize;

	Status CurrentStatus;
	bool Blocking;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

protected:
	virtual void load();

	virtual void emitUpdated();

	virtual void protocolRegistered(ProtocolFactory *protocolFactory);
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory);

	virtual void detailsAdded();
	virtual void detailsAboutToBeRemoved();
	virtual void detailsRemoved();

public:
	static ContactShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);
	static ContactShared * loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);

	explicit ContactShared(QUuid uuid = QUuid());
	virtual ~ContactShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	KaduShared_Property(Account, contactAccount, ContactAccount)
	KaduShared_Property(Avatar, contactAvatar, ContactAvatar)
	KaduShared_PropertyRead(Buddy, ownerBuddy, OwnerBuddy)
	void setOwnerBuddy(Buddy buddy);

	KaduShared_PropertyRead(QString, id, Id)
	void setId(const QString &id);

	KaduShared_Property(int, priority, Priority)
	KaduShared_Property(Status, currentStatus, CurrentStatus)
	KaduShared_PropertyBool(Blocking)
	KaduShared_Property(QString, protocolVersion, ProtocolVersion)
	KaduShared_Property(QHostAddress, address, Address)
	KaduShared_Property(unsigned int, port, Port)
	KaduShared_Property(QString, dnsName, DnsName)
	KaduShared_Property(short int, maximumImageSize, MaximumImageSize)

signals:
	void aboutToBeDetached();
	void detached();
	void aboutToBeAttached();
	void attached();
	void reattached();

	void updated();
	void idChanged(const QString &oldId);

};

#endif // CONTACT_SHARED_H
