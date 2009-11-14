/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SHARED_H
#define CONTACT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "buddies/avatar.h"
#include "buddies/buddy.h"
#include "contacts/contact-type.h"
#include "status/status.h"

#include "configuration/uuid-storable-object.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(type name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \

#undef PropertyBool
#define PropertyBool(capitalized_name) \
	bool is##capitalized_name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(bool name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }

class ContactDetails;

class KADUAPI ContactShared : public QObject, public UuidStorableObject, public QSharedData
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactShared)

	QUuid Uuid;
	ContactType Type;

	int BlockUpdatedSignalCount;
	bool Updated;

	ContactDetails *Details;

	Account ContactAccount;
	Avatar ContactAvatar;
	Buddy OwnerBuddy;
	QString Id;

	Status CurrentStatus;

	QString ProtocolVersion;

	QHostAddress Address;
	unsigned int Port;
	QString DnsName;

	void dataUpdated();
	void emitUpdated();

public:
	static ContactShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit ContactShared(ContactType type, QUuid uuid = QUuid());
	virtual ~ContactShared();

	virtual void load();
	virtual void store();

	void loadDetails();
	void unloadDetails();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	// contact type
	bool isNull() const { return ContactTypeNull == Type; }
	
	Property(ContactDetails *, details, Details)
	Property(Account, contactAccount, ContactAccount)
	Property(Avatar &, contactAvatar, ContactAvatar)
	PropertyRead(Buddy, ownerBuddy, OwnerBuddy)
	void setOwnerBuddy(Buddy buddy);

	PropertyRead(QString, id, Id)
	void setId(const QString &id);

	Property(Status, currentStatus, CurrentStatus)
	Property(QString, protocolVersion, ProtocolVersion)
	Property(QHostAddress, address, Address)
	Property(unsigned int, port, Port)
	Property(QString, dnsName, DnsName)

signals:
	void updated();

	void idChanged(const QString &id);

};

#endif // CONTACT_SHARED_H
