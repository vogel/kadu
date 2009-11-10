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

Contact::Contact(Account account, Buddy buddy, const QString &id, bool loaded) :
		UuidStorableObject("Contact", ContactManager::instance(), loaded),
		ContactAccount(account), OwnerBuddy(buddy), Id(id),
		ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
	Uuid = QUuid::createUuid();
}

Contact::Contact(StoragePoint *storage) :
		UuidStorableObject(storage), Uuid(QUuid::createUuid()),
		ContactAccount(0), OwnerBuddy(Buddy::null), ContactAvatar(this, false) /* TODO: 0.6.6 */, Blocked(false), OfflineTo(false), Port(0)
{
}

void Contact::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Uuid = loadAttribute<QString>("uuid");
	Id = loadValue<QString>("Id");

	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));
	QString buddyUuid = loadValue<QString>("Buddy");
	if (buddyUuid.isNull())
		buddyUuid = loadValue<QString>("Contact");
	setBuddy(BuddyManager::instance()->byUuid(buddyUuid));

	ContactAvatar.load();

	ContactManager::instance()->addContact(this);
}

void Contact::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("uuid", Uuid.toString(), true);
	storeValue("Id", Id);
	storeValue("Account", ContactAccount.uuid().toString());
	storeValue("Buddy", OwnerBuddy.uuid().toString());
	removeValue("Contact");

	ContactAvatar.store();
}

void Contact::setBuddy(Buddy buddy)
{
	if (buddy == OwnerBuddy)
		return;

	if (!OwnerBuddy.isNull())
		OwnerBuddy.removeContact(this);
	OwnerBuddy = buddy;
	if (!OwnerBuddy.isNull())
		OwnerBuddy.addContact(this);
}

void Contact::setId(const QString &newId)
{
	if (Id == newId)
		return;

	QString oldId = Id;
	Id = newId;

	emit idChanged(oldId);
}

bool Contact::isValid()
{
	ensureLoaded();

	return validateId();
}

void Contact::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}
