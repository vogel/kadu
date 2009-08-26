/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/xml-configuration-file.h"

#include "dnshandler.h"

#include "configuration/storage-point.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData(Contact contact, Account *account, const QString &id, bool loadFromConfiguration)
	: ContactAccount(account), OwnerContact(contact), ContactAvatar(this), Id(id), Blocked(false), OfflineTo(false), Port(0)
{
	if (!loadFromConfiguration)
		StorableObject::setLoaded(true);
	// TODO: 0.6.6 by loadFromStorage, please
	if (id.isNull())
		load();
}

StoragePoint * ContactAccountData::createStoragePoint()
{
	return ContactAccount
		? OwnerContact.storagePointForAccountData(ContactAccount)
		: 0;
}

void ContactAccountData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();
	Id = loadValue<QString>("Id");

	ContactAvatar.load();
}

void ContactAccountData::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();
	storeValue("Id", Id);

	ContactAvatar.store();
}

void ContactAccountData::setId(const QString &newId)
{
	if (Id == newId)
		return;

	QString oldId = Id;
	Id = newId;

	emit idChanged(oldId);
}

bool ContactAccountData::isValid()
{
	ensureLoaded();

	return validateId();
}

void ContactAccountData::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}
