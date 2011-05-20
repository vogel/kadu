/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class KADUAPI ContactShared : public QObject, public Shared, public DetailsHolder<ContactDetails>, ProtocolsAwareObject
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
	bool Dirty;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

	void detach(bool reattaching, bool emitSignals);
	void attach(bool reattaching, bool emitSignals);

	void doSetOwnerBuddy(const Buddy &buddy, bool emitSignals);

protected:
	virtual void load();

	virtual void emitUpdated();

	virtual void protocolRegistered(ProtocolFactory *protocolFactory);
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory);

	virtual void detailsAdded();
	virtual void afterDetailsAdded();
	virtual void detailsAboutToBeRemoved();
	virtual void detailsRemoved();

public:
	static ContactShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);
	static ContactShared * loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);

	explicit ContactShared(const QUuid &uuid = QUuid());
	virtual ~ContactShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void store();
	virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	KaduShared_PropertyRead(const Account &, contactAccount, ContactAccount)
	void setContactAccount(const Account &account);

	KaduShared_Property(const Avatar &, contactAvatar, ContactAvatar)
	KaduShared_PropertyRead(const Buddy &, ownerBuddy, OwnerBuddy)
	void setOwnerBuddy(const Buddy &buddy);

	KaduShared_PropertyRead(const QString &, id, Id)
	void setId(const QString &id);

	KaduShared_PropertyBoolRead(Dirty)
	void setDirty(bool dirty);

	KaduShared_Property(int, priority, Priority)
	KaduShared_Property(const Status &, currentStatus, CurrentStatus)
	KaduShared_PropertyBool(Blocking)
	KaduShared_Property(const QString &, protocolVersion, ProtocolVersion)
	KaduShared_Property(const QHostAddress &, address, Address)
	KaduShared_Property(unsigned int, port, Port)
	KaduShared_Property(const QString &, dnsName, DnsName)
	KaduShared_Property(short int, maximumImageSize, MaximumImageSize)

signals:
	void aboutToBeDetached(bool reattaching);
	void detached(const Buddy &previousBuddy);
	void aboutToBeAttached(const Buddy &nearFutureBuddy);
	void attached(bool reattached);

	void updated();
	void idChanged(const QString &oldId);
	void dirtinessChanged();

};

#endif // CONTACT_SHARED_H
