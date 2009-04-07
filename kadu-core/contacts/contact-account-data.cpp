/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dnshandler.h"
#include "xml_config_file.h"

#include "configuration/storage-point.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData(Contact contact, Account *account, const QString &id)
	: ContactAccount(account), OwnerContact(contact), Id(id), Blocked(false), OfflineTo(false), Port(0)
{
	// TODO: 0.6.6 by loadFromStorage, please
	if (id.isNull())
		loadConfiguration();
}

StoragePoint * ContactAccountData::createStoragePoint()
{
	return ContactAccount
		? OwnerContact.storagePointForAccountData(ContactAccount)
		: 0;
}

void ContactAccountData::loadConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return;

	Id = sp->storage()->getTextNode(sp->point(), "Id");
}

void ContactAccountData::store()
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return;

	sp->storage()->createTextNode(sp->point(), "Id", Id);
}

bool ContactAccountData::isValid()
{
	return validateId();
}

void ContactAccountData::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}
