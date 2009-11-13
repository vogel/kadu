/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"

#include "contact-shared.h"

ContactShared * ContactShared::loadFromStorage(StoragePoint *storagePoint)
{
	ContactShared *result = new ContactShared(ContactTypeNormal);
	result->setStorage(storagePoint);
	result->load();

	return result;
}

ContactShared::ContactShared(ContactType type, QUuid uuid) :
		UuidStorableObject("Account", ContactManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Type(type),
		BlockUpdatedSignalCount(0), Updated(false),
		Details(0), ContactAvatar(Contact(this), false) /* TODO: 0.6.6 */
{
}

ContactShared::~ContactShared()
{
}

void ContactShared::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();

	Uuid = QUuid(loadAttribute<QString>("uuid"));
	Id = loadValue<QString>("Id");

	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));
	QString buddyUuid = loadValue<QString>("Buddy");
	if (buddyUuid.isNull())
		buddyUuid = loadValue<QString>("Contact");
	setOwnerBuddy(BuddyManager::instance()->byUuid(buddyUuid));

	ContactAvatar.load();

// 	ContactManager::instance()->addContact(new Contact(this));
}

void ContactShared::store()
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

void ContactShared::loadDetails()
{
	if (Details)
		return;

	if (ContactAccount.isNull())
		return;

	Protocol *protocol = ContactAccount.protocolHandler();
	if (!protocol)
		return;

	ProtocolFactory *factory = protocol->protocolFactory();
	if (!factory)
		return;

	Details = factory->createContactDetails(Contact(this));
}

void ContactShared::unloadDetails()
{
	if (Details)
	{
		delete Details;
		Details = 0;
	}
}

void ContactShared::dataUpdated()
{
	Updated = true;
	emitUpdated();
}

void ContactShared::emitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emit updated();
		Updated = false;
	}
}

void ContactShared::setOwnerBuddy(Buddy buddy)
{
	if (!OwnerBuddy.isNull())
		OwnerBuddy.removeContact(Contact(this));
	OwnerBuddy = buddy;
	if (!OwnerBuddy.isNull())
		OwnerBuddy.addContact(Contact(this));

	dataUpdated();
}

void ContactShared::setId(const QString &id)
{
	if (Id == id)
		return;
	
	QString oldId = Id;
	Id = id;
	
	emit idChanged(oldId);
}
