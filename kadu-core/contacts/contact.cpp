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
#include "buddies/buddy-manager.h"
#include "dnshandler.h"

#include "contact.h"

Contact Contact::null(ContactShared::TypeNull);

Contact Contact::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return Contact(ContactShared::loadFromStorage(accountStoragePoint));
}

Contact::Contact(ContactShared::ContactType type) :
		Data(ContactShared::TypeNull != type ? new ContactShared(type) : 0)
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
