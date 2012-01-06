/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtNetwork/QHostAddress>

#include "contacts/contact-details.h"
#include "status/status.h"
#include "storage/shared.h"

class Account;
class Avatar;
class Buddy;
class ProtocolFactory;

class KADUAPI ContactShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactShared)

	ContactDetails *Details;

	Account *ContactAccount;
	Avatar *ContactAvatar;
	Buddy *OwnerBuddy;
	QString Id;
	int Priority;
	short int MaximumImageSize;
	quint16 UnreadMessagesCount;

	Status CurrentStatus;
	bool Blocking;
	bool Dirty;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

	void deleteDetails();

	void detach(bool resetBuddy, bool reattaching, bool emitSignals);
	void attach(const Buddy &buddy, bool reattaching, bool emitSignals);

	void doSetContactAvatar(const Avatar &contactAvatar);
	void doSetOwnerBuddy(const Buddy &buddy, bool emitSignals);

private slots:
	void protocolFactoryRegistered(ProtocolFactory *protocolFactory);
	void protocolFactoryUnregistered(ProtocolFactory *protocolFactory);

	void avatarUpdated();

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

	virtual void emitUpdated();

public:
	static ContactShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);
	static ContactShared * loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint);

	explicit ContactShared(const QUuid &uuid = QUuid());
	virtual ~ContactShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	virtual void aboutToBeRemoved();

	ContactDetails * details() const { return Details; }

	KaduShared_PropertyRead(const QString &, id, Id)
	void setId(const QString &id);

	KaduShared_PropertyBoolRead(Dirty)
	void setDirty(bool dirty);

	KaduShared_PropertyDeclCRW(Account, contactAccount, ContactAccount)

	KaduShared_PropertyReadDecl(Avatar, contactAvatar)
	void setContactAvatar(const Avatar &contactAvatar);

	KaduShared_PropertyDeclCRW(Buddy, ownerBuddy, OwnerBuddy)
	void removeOwnerBuddy();

	KaduShared_Property(int, priority, Priority)
	KaduShared_Property(const Status &, currentStatus, CurrentStatus)
	KaduShared_PropertyBool(Blocking)
	KaduShared_Property(const QString &, protocolVersion, ProtocolVersion)
	KaduShared_Property(const QHostAddress &, address, Address)
	KaduShared_Property(unsigned int, port, Port)
	KaduShared_Property(const QString &, dnsName, DnsName)
	KaduShared_Property(short int, maximumImageSize, MaximumImageSize)
	KaduShared_Property(quint16, unreadMessagesCount, UnreadMessagesCount)

	bool isAnonymous();
	QString display(bool useBuddyData);
	Avatar avatar(bool useBuddyData);

signals:
	void aboutToBeDetached();
	void detached(const Buddy &previousBuddy, bool reattaching);
	void aboutToBeAttached(const Buddy &nearFutureBuddy);
	void attached(bool reattached);

	void updated();
	void idChanged(const QString &oldId);
	void dirtinessChanged();

};

// for MOC
#include "buddies/buddy.h"

#endif // CONTACT_SHARED_H

