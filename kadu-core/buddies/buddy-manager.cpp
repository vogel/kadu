/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-remove-predicate-object.h"
#include "buddies/group-manager.h"
#include "buddies/account-data/contact-account-data.h"
#include "configuration/configuration-manager.h"
#include "configuration/storage-point.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"

#include "debug.h"
#include "group.h"

#include "buddy-manager.h"

BuddyManager * BuddyManager::Instance = 0;

BuddyManager * BuddyManager::instance()
{
	if (0 == Instance)
		Instance = new BuddyManager();

	return Instance;
}

BuddyManager::BuddyManager()
{
	Core::instance()->configuration()->registerStorableObject(this);

	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group *)),
			this, SLOT(groupRemoved(Group *)));
}

BuddyManager::~BuddyManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);
}

StoragePoint * BuddyManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("ContactsNew"));
}

void BuddyManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QDomNodeList contactsNodes = configurationStorage->getNodes(contactsNode, "Contact");
	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactsNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;

		Buddy contact;
		contact.importConfiguration(configurationStorage, contactElement);

		addBuddy(contact);
	}
}

void BuddyManager::load()
{
	StorableObject::load();

	if (xml_config_file->getNode("ContactsNew", XmlConfigFile::ModeFind).isNull())
	{
		importConfiguration(xml_config_file);
		return;
	}

	if (!isValidStorage())
		return;

	QDomElement contactsNewNode = storage()->point();
	QDomNodeList contactsNodes = contactsNewNode.elementsByTagName("Contact");

	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomNode contactNode = contactsNodes.at(i);
		QDomElement contactElement = contactNode.toElement();
		if (contactElement.isNull())
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(storage()->storage(), contactElement);
		addBuddy(Buddy::loadFromStorage(contactStoragePoint));
	}
}

void BuddyManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	foreach (Buddy contact, Contacts)
		contact.store();
}

void BuddyManager::addBuddy(Buddy contact)
{
	if (contact.isNull())
		return;

	ensureLoaded();

	if (Contacts.contains(contact))
	{
		contact.setType(BuddyShared::TypeNormal);
		return;
	}

	emit buddyAboutToBeAdded(contact);
	Contacts.append(contact);
	emit buddyAdded(contact);

	connect(contact.data(), SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	connect(contact.data(), SIGNAL(accountDataAboutToBeAdded(Account)),
			this, SLOT(contactAccountDataAboutToBeAdded(Account)));
	connect(contact.data(), SIGNAL(accountDataAdded(Account)),
			this, SLOT(contactAccountDataAdded(Account)));
	connect(contact.data(), SIGNAL(accountDataAboutToBeRemoved(Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Account)));
	connect(contact.data(), SIGNAL(accountDataRemoved(Account)),
			this, SLOT(contactAccountDataRemoved(Account)));
	connect(contact.data(), SIGNAL(accountDataIdChanged(Account, const QString &)),
			this, SLOT(contactAccountDataIdChanged(Account, const QString &)));
}

void BuddyManager::removeBuddy(Buddy contact)
{
	kdebugf();
	if (contact.isNull())
		return;

	ensureLoaded();

	if (!Contacts.contains(contact))
		return;

	disconnect(contact.data(), SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	disconnect(contact.data(), SIGNAL(accountDataAboutToBeAdded(Account)),
			this, SLOT(contactAccountDataAboutToBeAdded(Account)));
	disconnect(contact.data(), SIGNAL(accountDataAdded(Account)),
			this, SLOT(contactAccountDataAdded(Account)));
	disconnect(contact.data(), SIGNAL(accountDataAboutToBeRemoved(Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Account)));
	disconnect(contact.data(), SIGNAL(accountDataRemoved(Account)),
			this, SLOT(contactAccountDataRemoved(Account)));
	disconnect(contact.data(), SIGNAL(accountDataIdChanged(Account, const QString &)),
			this, SLOT(contactAccountDataIdChanged(Account, const QString &)));

	emit buddyAboutToBeRemoved(contact);
	if (BuddyRemovePredicateObject::inquireAll(contact))
	{
		Contacts.removeAll(contact);
		contact.removeFromStorage();
	}
	emit buddyRemoved(contact);
	contact.setType(BuddyShared::TypeAnonymous);

	kdebugf();
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	while (source.accounts().size())
	{
		ContactAccountData *cad = source.accountData(source.accounts()[0]);
		cad->setContact(destination);
	}

	source.setType(BuddyShared::TypeAnonymous);
	removeBuddy(source);

	source.data()->setUuid(destination.uuid()); // just for case
	source.setData(destination.data()); // TODO: 0.8 tricky merge, this should work well ;)
	
	Core::instance()->configuration()->flush();
}

Buddy BuddyManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return Buddy::null;

	ensureLoaded();

	return Contacts.at(index);
}

Buddy BuddyManager::byId(Account account, const QString &id)
{
	if (id.isEmpty() || account.isNull())
		return Buddy::null;

	ensureLoaded();

	foreach (Buddy contact, Contacts)
	{
		if (id == contact.id(account))
			return contact;
	}

	Buddy anonymous = account.createAnonymous(id);
	addBuddy(anonymous);

	return anonymous;
}

Buddy BuddyManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return Buddy::null;

	ensureLoaded();

	foreach (Buddy contact, Contacts)
		if (uuid == contact.uuid().toString())
			return contact;

	return Buddy::null;
}

Buddy BuddyManager::byDisplay(const QString &display)
{
	if (display.isEmpty())
		return Buddy::null;

	ensureLoaded();

	foreach (Buddy contact, Contacts)
	{
		if (display == contact.display())
			return contact;
	}

	return Buddy::null;
}

void BuddyManager::blockUpdatedSignal(Buddy &contact)
{
	contact.data()->blockUpdatedSignal();
}

void BuddyManager::unblockUpdatedSignal(Buddy &contact)
{
	contact.data()->unblockUpdatedSignal();
}

BuddyList BuddyManager::buddies()
{
	ensureLoaded();
	return Contacts;
}

BuddyList BuddyManager::buddies(Account account, bool includeAnonymous)
{
	BuddyList result;

	foreach (Buddy contact, Contacts)
		if (contact.accountData(account) && (includeAnonymous || !contact.isAnonymous()))
			result << contact;

	ensureLoaded();

	return result;
}

const Buddy & BuddyManager::byBuddyShared(BuddyShared *data)
{
	foreach (const Buddy &contact, Contacts)
		if (data == contact.data())
			return contact;

	return Buddy::null;
}

void BuddyManager::buddyDataUpdated()
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit buddyUpdated(contact);
}

void BuddyManager::contactAccountDataAboutToBeAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit contactAccountDataAboutToBeAdded(contact, account);
}

void BuddyManager::contactAccountDataAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit contactAccountDataAdded(contact, account);
}

void BuddyManager::contactAccountDataAboutToBeRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit contactAccountDataAboutToBeRemoved(contact, account);
}

void BuddyManager::contactAccountDataRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit contactAccountDataRemoved(contact, account);
}

void BuddyManager::contactAccountDataIdChanged(Account account, const QString &oldId)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy contact = byBuddyShared(cd);
	if (!contact.isNull())
		emit contactAccountIdChanged(contact, account, oldId);
}

void BuddyManager::groupRemoved(Group *group)
{
	foreach (Buddy contact, Contacts)
		contact.removeFromGroup(group);
}
