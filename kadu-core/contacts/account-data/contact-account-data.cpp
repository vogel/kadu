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
#include "dnshandler.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData(Account *account, Contact contact, const QString &id, bool loaded) :
		UuidStorableObject("ContactAccountData", loaded), ContactAccount(account), OwnerContact(contact), Id(id),
		ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
	Uuid = QUuid::createUuid();
}

ContactAccountData::ContactAccountData(Account *account, Contact contact, const QString &id, StoragePoint *storage) :
		UuidStorableObject(storage), Uuid(QUuid::createUuid()),
		ContactAccount(account), OwnerContact(contact), Id(id),
		ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
}

void ContactAccountData::recreateStoragePoint()
{
	StoragePoint *oldStorage = storage();
	QDomElement oldPoint = oldStorage->point();
	oldPoint.parentNode().removeChild(oldPoint);

	setStorage(contact().storagePointForAccountData(ContactAccount));
}

void ContactAccountData::load()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	StorableObject::load();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	Uuid = loadAttribute<QString>("uuid");

	// TODO: remove after 0.7 release, this is only
	// for compatibility with older 0.6.6 versions
	if (loadValue<QString>("Account").isEmpty())
		Uuid = QUuid::createUuid();

	Id = loadValue<QString>("Id");

	ContactAvatar.load();
}

void ContactAccountData::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();
	storeValue("uuid", Uuid.toString(), true);
	storeValue("Id", Id);
	storeValue("Account", ContactAccount->uuid().toString());

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
