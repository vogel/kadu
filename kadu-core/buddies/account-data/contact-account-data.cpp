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
#include "buddies/account-data/contact-account-data-manager.h"
#include "buddies/buddy-manager.h"
#include "dnshandler.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData(Account account, Contact contact, const QString &id, bool loaded) :
		UuidStorableObject("ContactAccountData", ContactAccountDataManager::instance(), loaded),
		ContactAccount(account), OwnerContact(contact), Id(id),
		ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
	Uuid = QUuid::createUuid();
}

ContactAccountData::ContactAccountData(StoragePoint *storage) :
		UuidStorableObject(storage), Uuid(QUuid::createUuid()),
		ContactAccount(0), OwnerContact(Contact::null), ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
}

void ContactAccountData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Uuid = loadAttribute<QString>("uuid");
	Id = loadValue<QString>("Id");
	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));
	setContact(ContactManager::instance()->byUuid(loadValue<QString>("Contact")));

	ContactAvatar.load();

	ContactAccountDataManager::instance()->addContactAccountData(this);
}

void ContactAccountData::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("uuid", Uuid.toString(), true);
	storeValue("Id", Id);
	storeValue("Account", ContactAccount.uuid().toString());
	storeValue("Contact", OwnerContact.uuid().toString());

	ContactAvatar.store();
}

void ContactAccountData::setContact(Contact contact)
{
	if (contact == OwnerContact)
		return;

	if (!OwnerContact.isNull())
		OwnerContact.removeAccountData(this);
	OwnerContact = contact;
	if (!OwnerContact.isNull())
		OwnerContact.addAccountData(this);
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
