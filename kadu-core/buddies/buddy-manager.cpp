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
	{
		Instance = new BuddyManager();
		Instance->init();
	}

	return Instance;
}

BuddyManager::BuddyManager()
{
}

BuddyManager::~BuddyManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void BuddyManager::init()
{
	ConfigurationManager::instance()->registerStorableObject(this);

	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
}

StoragePoint * BuddyManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Buddies"));
}

void BuddyManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("OldContacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactElements = configurationStorage->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
	{
		Buddy buddy;
		buddy.importConfiguration(configurationStorage, contactElement);

		addBuddy(buddy);
	}
}

void BuddyManager::load()
{
	if (!needsLoad())
		return;

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
		buddy.setAnonymous(false);
		return;
	}

	emit buddyAboutToBeAdded(buddy);
	Buddies.append(buddy);
	emit buddyAdded(buddy);

	connect(buddy.data(), SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	connect(buddy.data(), SIGNAL(contactAboutToBeAdded(Account)),
			this, SLOT(contactAboutToBeAdded(Account)));
	connect(buddy.data(), SIGNAL(contactAdded(Account)),
			this, SLOT(contactAdded(Account)));
	connect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Account)),
			this, SLOT(contactAboutToBeRemoved(Account)));
	connect(buddy.data(), SIGNAL(contactRemoved(Account)),
			this, SLOT(contactRemoved(Account)));
	connect(buddy.data(), SIGNAL(contactIdChanged(Account, const QString &)),
			this, SLOT(contactIdChanged(Account, const QString &)));
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
			this, SLOT(contactAboutToBeAdded(Account)));
	disconnect(buddy.data(), SIGNAL(contactAdded(Account)),
			this, SLOT(contactAdded(Account)));
	disconnect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Account)),
			this, SLOT(contactAboutToBeRemoved(Account)));
	disconnect(buddy.data(), SIGNAL(contactRemoved(Account)),
			this, SLOT(contactRemoved(Account)));
	disconnect(buddy.data(), SIGNAL(contactIdChanged(Account, const QString &)),
			this, SLOT(contactIdChanged(Account, const QString &)));

	emit buddyAboutToBeRemoved(buddy);
	if (BuddyRemovePredicateObject::inquireAll(buddy))
	{
		Buddies.removeAll(buddy);
		buddy.removeFromStorage();
	}
	emit buddyRemoved(buddy);
	buddy.setAnonymous(true);

	kdebugf();
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	ensureLoaded();

	while (source.accounts().size())
	{
		Contact contact = source.contact(source.accounts()[0]);
		contact.setOwnerBuddy(destination);
	}

	source.setAnonymous(true);
	removeBuddy(source);

	source.data()->setUuid(destination.uuid()); // just for case
// 	source.data() setData(destination.data()); // TODO: 0.6.6 tricky merge, this should work well ;)
	
	ConfigurationManager::instance()->flush();
}

Buddy BuddyManager::byIndex(unsigned int index)
{
	ensureLoaded();

	if (index < 0 || index >= count())
		return Buddy::null;

	return Buddies.at(index);
}

Buddy BuddyManager::byId(Account account, const QString &id)
{
	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Buddy::null;

	foreach (Buddy buddy, Buddies)
	{
		if (id == buddy.id(account))
			return buddy;
	}

	Buddy anonymous = account.createAnonymous(id);
	addBuddy(anonymous);

	return anonymous;
}

Buddy BuddyManager::byUuid(const QString &uuidString)
{
	ensureLoaded();

	QUuid uuid(uuidString);
	if (uuid.isNull())
		return Buddy::null;

	foreach (Buddy buddy, Buddies)
		if (uuid == buddy.uuid())
			return buddy;

	return Buddy::null;
}

Buddy BuddyManager::byDisplay(const QString &display)
{
	ensureLoaded();

	if (display.isEmpty())
		return Buddy::null;

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
	ensureLoaded();

	BuddyList result;

	foreach (Buddy buddy, Buddies)
		if (!buddy.contact(account).isNull() && (includeAnonymous || !buddy.isAnonymous()))
			result << buddy;

	return result;
}

const Buddy & BuddyManager::byBuddyShared(BuddyShared *data)
{
	ensureLoaded();

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

void BuddyManager::contactAboutToBeAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAboutToBeAdded(buddy, account);
}

void BuddyManager::contactAdded(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAdded(buddy, account);
}

void BuddyManager::contactAboutToBeRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAboutToBeRemoved(buddy, account);
}

void BuddyManager::contactRemoved(Account account)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactRemoved(buddy, account);
}

void BuddyManager::contactIdChanged(Account account, const QString &oldId)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactIdChanged(buddy, account, oldId);
}

void BuddyManager::groupRemoved(Group group)
{
	foreach (Buddy buddy, Buddies)
		buddy.removeFromGroup(group);
}
