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

Contact Contact::null(true);

Contact Contact::loadFromStorage(StoragePoint *contactStoragePoint)
{
	return Contact(ContactShared::loadFromStorage(contactStoragePoint));
}

Contact::Contact(bool null) :
		SharedBase<ContactShared>(null)
{
}

Contact::Contact()
{
	data()->setState(StorableObject::StateNew);
}

Contact::Contact(ContactShared *data) :
		SharedBase<ContactShared>(data)
{
}

Contact::Contact(const Contact &copy) :
		SharedBase<ContactShared>(copy)
{
}

Contact::~Contact()
{
}

Contact & Contact::operator = (const Contact &copy)
{
	SharedBase<ContactShared>::operator=(copy);
	return *this;
}

void Contact::loadDetails()
{
	if (!isNull())
		data()->loadDetails();
}

void Contact::unloadDetails()
{
	if (!isNull())
		data()->unloadDetails();
}

void Contact::connectDataSignals()
{
	if (isNull())
		return;

	connect(data(), SIGNAL(idChanged(const QString &)),
			this, SIGNAL(idChanged(const QString &)));
}

void Contact::disconnectDataSignals()
{
	if (isNull())
		return;

	disconnect(data(), SIGNAL(idChanged(const QString &)),
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

KaduSharedBase_PropertyDef(Contact, ContactDetails *, details, Details, 0)
KaduSharedBase_PropertyReadDef(Contact, QUuid, uuid, Uuid, QUuid())
KaduSharedBase_PropertyReadDef(Contact, StoragePoint *, storage, Storage, 0)
KaduSharedBase_PropertyDef(Contact, Account, contactAccount, ContactAccount, Account::null)
KaduSharedBase_PropertyDef(Contact, Avatar, contactAvatar, ContactAvatar, Avatar::null)
KaduSharedBase_PropertyDef(Contact, Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
KaduSharedBase_PropertyDef(Contact, QString, id, Id, QString::null)
KaduSharedBase_PropertyDef(Contact, Status, currentStatus, CurrentStatus, Status::null)
KaduSharedBase_PropertyDef(Contact, QString, protocolVersion, ProtocolVersion, QString::null)
KaduSharedBase_PropertyDef(Contact, QHostAddress, address, Address, QHostAddress())
KaduSharedBase_PropertyDef(Contact, unsigned int, port, Port, 0)
KaduSharedBase_PropertyDef(Contact, QString, dnsName, DnsName, QString::null)
