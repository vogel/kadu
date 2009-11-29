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
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "storage/storage-point.h"

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
	connect(buddy.data(), SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	connect(buddy.data(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(buddy.data(), SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
	connect(buddy.data(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
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
	disconnect(buddy.data(), SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	disconnect(buddy.data(), SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	disconnect(buddy.data(), SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	disconnect(buddy.data(), SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
	disconnect(buddy.data(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));

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

	foreach (const Contact &contact, source.contacts())
		contact.setOwnerBuddy(destination);

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

	foreach (const Buddy &buddy, Buddies)
		if (buddy.hasContact(account) && (includeAnonymous || !buddy.isAnonymous()))
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

void BuddyManager::contactAboutToBeAdded(Contact contact)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAboutToBeAdded(buddy, contact.contactAccount());
}

void BuddyManager::contactAdded(Contact contact)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAdded(buddy, contact.contactAccount());
}

void BuddyManager::contactAboutToBeRemoved(Contact contact)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactAboutToBeRemoved(buddy, contact.contactAccount());
}

void BuddyManager::contactRemoved(Contact contact)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactRemoved(buddy, contact.contactAccount());
}

void BuddyManager::contactIdChanged(Contact contact, const QString &oldId)
{
	BuddyShared *cd = dynamic_cast<BuddyShared *>(sender());
	if (!cd)
		return;

	Buddy buddy = byBuddyShared(cd);
	if (!buddy.isNull())
		emit contactIdChanged(buddy, contact.contactAccount(), oldId);
}

void BuddyManager::groupRemoved(Group group)
{
	foreach (Buddy buddy, Buddies)
		buddy.removeFromGroup(group);
}
