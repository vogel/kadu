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
#include "contacts/contact.h"
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
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Buddies"));
}

void BuddyManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("Buddies", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QDomNodeList contactsNodes = configurationStorage->getNodes(contactsNode, "Contact");
	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactsNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;

		Buddy buddy;
		buddy.importConfiguration(configurationStorage, contactElement);

		addBuddy(buddy);
	}
}

void BuddyManager::load()
{
	StorableObject::load();

	if (xml_config_file->getNode("Buddies", XmlConfigFile::ModeFind).isNull())
	{
		importConfiguration(xml_config_file);
		return;
	}

	if (!isValidStorage())
		return;

	QDomElement contactsNewNode = storage()->point();
	QDomNodeList contactsNodes = contactsNewNode.elementsByTagName("Buddy");

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

	foreach (Buddy buddy, Buddies)
		buddy.store();
}

void BuddyManager::addBuddy(Buddy buddy)
{
	if (buddy.isNull())
		return;

	ensureLoaded();

	if (Buddies.contains(buddy))
	{
		buddy.setType(BuddyShared::TypeNormal);
		return;
	}

	emit buddyAboutToBeAdded(buddy);
	Buddies.append(buddy);
	emit buddyAdded(buddy);

	connect(buddy.data(), SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	connect(buddy.data(), SIGNAL(contactAboutToBeAdded(Account)),
			this, SLOT(contactAccountDataAboutToBeAdded(Account)));
	connect(buddy.data(), SIGNAL(contactAdded(Account)),
			this, SLOT(contactAccountDataAdded(Account)));
	connect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Account)));
	connect(buddy.data(), SIGNAL(contactRemoved(Account)),
			this, SLOT(contactAccountDataRemoved(Account)));
	connect(buddy.data(), SIGNAL(contactIdChanged(Account, const QString &)),
			this, SLOT(contactAccountDataIdChanged(Account, const QString &)));
}

void BuddyManager::removeBuddy(Buddy buddy)
{
	kdebugf();
	if (buddy.isNull())
		return;

	ensureLoaded();

	if (!Buddies.contains(buddy))
		return;

	disconnect(buddy.data(), SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	disconnect(buddy.data(), SIGNAL(contactAboutToBeAdded(Account)),
			this, SLOT(contactAccountDataAboutToBeAdded(Account)));
	disconnect(buddy.data(), SIGNAL(contactAdded(Account)),
			this, SLOT(contactAccountDataAdded(Account)));
	disconnect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Account)),
			this, SLOT(contactAccountDataAboutToBeRemoved(Account)));
	disconnect(buddy.data(), SIGNAL(contactRemoved(Account)),
			this, SLOT(contactAccountDataRemoved(Account)));
	disconnect(buddy.data(), SIGNAL(contactIdChanged(Account, const QString &)),
			this, SLOT(contactAccountDataIdChanged(Account, const QString &)));

	emit buddyAboutToBeRemoved(buddy);
	if (BuddyRemovePredicateObject::inquireAll(buddy))
	{
		Buddies.removeAll(buddy);
		buddy.removeFromStorage();
	}
	emit buddyRemoved(buddy);
	buddy.setType(BuddyShared::TypeAnonymous);

	kdebugf();
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	while (source.accounts().size())
	{
		Contact *cad = source.contact(source.accounts()[0]);
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

	return Buddies.at(index);
}

Buddy BuddyManager::byId(Account account, const QString &id)
{
	if (id.isEmpty() || account.isNull())
		return Buddy::null;

	ensureLoaded();

	foreach (Buddy buddy, Buddies)
	{
		if (id == buddy.id(account))
			return buddy;
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

	foreach (Buddy buddy, Buddies)
		if (uuid == buddy.uuid().toString())
			return buddy;

	return Buddy::null;
}

Buddy BuddyManager::byDisplay(const QString &display)
{
	if (display.isEmpty())
		return Buddy::null;

	ensureLoaded();

	foreach (Buddy buddy, Buddies)
	{
		if (display == buddy.display())
			return buddy;
	}

	return Buddy::null;
}

void BuddyManager::blockUpdatedSignal(Buddy &buddy)
{
	buddy.data()->blockUpdatedSignal();
}

void BuddyManager::unblockUpdatedSignal(Buddy &buddy)
{
	buddy.data()->unblockUpdatedSignal();
}

BuddyList BuddyManager::buddies()
{
	ensureLoaded();
	return Buddies;
}

BuddyList BuddyManager::buddies(Account account, bool includeAnonymous)
{
	BuddyList result;

	foreach (Buddy buddy, Buddies)
		if (buddy.contact(account) && (includeAnonymous || !buddy.isAnonymous()))
			result << buddy;

	ensureLoaded();

	return result;
}

const Buddy & BuddyManager::byBuddyShared(BuddyShared *data)
{
	foreach (const Buddy &buddy, Buddies)
		if (data == buddy.data())
			return buddy;

	return Buddy::null;
}

void BuddyManager::buddyDataUpdated()
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit buddyUpdated(buddy);
}

void BuddyManager::contactAccountDataAboutToBeAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAccountDataAboutToBeAdded(buddy, account);
}

void BuddyManager::contactAccountDataAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAccountDataAdded(buddy, account);
}

void BuddyManager::contactAccountDataAboutToBeRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAccountDataAboutToBeRemoved(buddy, account);
}

void BuddyManager::contactAccountDataRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAccountDataRemoved(buddy, account);
}

void BuddyManager::contactAccountDataIdChanged(Account account, const QString &oldId)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAccountIdChanged(buddy, account, oldId);
}

void BuddyManager::groupRemoved(Group *group)
{
	foreach (Buddy buddy, Buddies)
		buddy.removeFromGroup(group);
}
