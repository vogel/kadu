/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/xml-configuration-file.h"
#include "configuration/storage-point.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "buddies/buddy-manager.h"
#include "dnshandler.h"

#include "contact.h"

Contact Contact::null(ContactTypeNull);

Contact Contact::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return Contact(ContactShared::loadFromStorage(accountStoragePoint));
}

Contact::Contact(ContactType type) :
		Data(ContactTypeNull != type ? new ContactShared(type) : 0)
{
	connectDataSignals();
}

Contact::Contact(ContactShared *data) :
		Data(data)
{
	connectDataSignals();
}

Contact::Contact(const Contact &copy) :
		Data(copy.Data)
{
	connectDataSignals();
}

Contact::~Contact()
{
	disconnectDataSignals();
}

bool Contact::isNull() const
{
	return !Data.data() || Data->isNull();
}

Contact & Contact::operator = (const Contact &copy)
{
	disconnectDataSignals();
	Data = copy.Data;
	connectDataSignals();

	return *this;
}

bool Contact::operator == (const Contact &compare) const
{
	return Data == compare.Data;
}

bool Contact::operator != (const Contact &compare) const
{
	return Data != compare.Data;
}

int Contact::operator < (const Contact& compare) const
{
	return Data.data() - compare.Data.data();
}

void Contact::store()
{
	if (Data)
		Data->store();
}

void Contact::loadDetails()
{
	if (Data)
		Data->loadDetails();
}

void Contact::unloadDetails()
{
	if (Data)
		Data->unloadDetails();
}

void Contact::connectDataSignals()
{
	if (isNull())
		return;

	connect(Data.data(), SIGNAL(idChanged(const QString &)),
			this, SIGNAL(idChanged(const QString &)));
}

void Contact::disconnectDataSignals()
{
	if (isNull())
		return;

	disconnect(Data.data(), SIGNAL(idChanged(const QString &)),
			this, SIGNAL(idChanged(const QString &)));
}
/*
void Contact::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}*/

bool Contact::validateId()
{
	return true;
}

bool Contact::isValid()
{
	return validateId();
}

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name, default) \
	type Contact::name() const\
	{\
		return !Data\
			? default\
			: Data->name();\
	}

#undef PropertyWrite
#define PropertyWrite(type, name, capitalized_name, default) \
	void Contact::set##capitalized_name(type name) const\
	{\
		if (Data)\
			Data->set##capitalized_name(name);\
	}

#undef Property
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

#undef PropertyBoolRead
#define PropertyBoolRead(capitalized_name, default) \
	bool Contact::is##capitalized_name() const\
	{\
		return !Data\
			? default\
			: Data->is##capitalized_name();\
	}

#undef PropertyBoolWrite
#define PropertyBoolWrite(capitalized_name, default) \
	void Contact::set##capitalized_name(bool name) const\
	{\
		if (Data)\
			Data->set##capitalized_name(name);\
	}

#undef PropertyBool
#define PropertyBool(capitalized_name, default) \
	PropertyBoolRead(capitalized_name, default) \
	PropertyBoolWrite(capitalized_name, default)

Property(ContactDetails *, details, Details, 0)
PropertyRead(QUuid, uuid, Uuid, QUuid())
PropertyRead(StoragePoint *, storage, Storage, 0)
Property(Account, contactAccount, ContactAccount, Account::null)

Avatar & Contact::contactAvatar() const
{
	return Data->contactAvatar();
}

Property(Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
Property(QString, id, Id, QString::null)
Property(Status, currentStatus, CurrentStatus, Status::null)
Property(QString, protocolVersion, ProtocolVersion, QString::null)
Property(QHostAddress, address, Address, QHostAddress())
Property(unsigned int, port, Port, 0)
Property(QString, dnsName, DnsName, QString::null)
PropertyBool(Blocked, false)
PropertyBool(OfflineTo, false)
