/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_H
#define CONTACT_H

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact-type.h"
#include "status/status.h"

#include "exports.h"

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name, default) \
	type name() const;

#undef PropertyWrite
#define PropertyWrite(type, name, capitalized_name, default) \
	void set##capitalized_name(type name) const;

#undef Property
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

#undef PropertyBoolRead
#define PropertyBoolRead(capitalized_name, default) \
	bool is##capitalized_name() const;

#undef PropertyBoolWrite
#define PropertyBoolWrite(capitalized_name, default) \
	void set##capitalized_name(bool name) const;

#undef PropertyBool
#define PropertyBool(capitalized_name, default) \
	PropertyBoolRead(capitalized_name, default) \
	PropertyBoolWrite(capitalized_name, default)

class Avatar;
class ContactDetails;
class ContactShared;
class XmlConfigFile;

class KADUAPI Contact : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<ContactShared> Data;

	void connectDataSignals();
	void disconnectDataSignals();

public:

	static Contact loadFromStorage(StoragePoint *storage);
	static Contact null;

	explicit Contact(ContactType type = ContactTypeNormal);
	explicit Contact(ContactShared *data);
	Contact(const Contact &copy);
	virtual ~Contact();

	ContactShared * data() const { return Data.data(); }

	bool isNull() const;

	Contact & operator = (const Contact &copy);
	bool operator == (const Contact &compare) const;
	bool operator != (const Contact &compare) const;
	int operator < (const Contact &compare) const;

	void store();

	void loadDetails();
	void unloadDetails();

	virtual bool validateId();
	bool isValid();

	Property(ContactDetails *, details, Details, 0)
	PropertyRead(QUuid, uuid, Uuid, QUuid())
	PropertyRead(StoragePoint *, storage, Storage, 0)
	Property(Account, contactAccount, ContactAccount, Account::null)
	Avatar & contactAvatar() const;
	Property(Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
	Property(QString, id, Id, QString::null)
	Property(Status, currentStatus, CurrentStatus, Status::null)
	Property(QString, protocolVersion, ProtocolVersion, QString::null)
	Property(QHostAddress, address, Address, QHostAddress())
	Property(unsigned int, port, Port, 0)
	Property(QString, dnsName, DnsName, QString::null)

signals:
	void idChanged(const QString &id);

};

Q_DECLARE_METATYPE(Contact)

#endif // CONTACT_H
