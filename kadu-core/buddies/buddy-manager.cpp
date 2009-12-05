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
}

void BuddyManager::init()
{
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
	
	if (xml_config_file->getNode("Buddies", XmlConfigFile::ModeFind).isNull())
		importConfiguration(xml_config_file);
}

void BuddyManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("OldContacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactElements = configurationStorage->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
	{
		Buddy buddy = Buddy::create();
		buddy.importConfiguration(configurationStorage, contactElement);

		addItem(buddy);
	}
}

void BuddyManager::load()
{
	SimpleManager<Buddy>::load();
}

void BuddyManager::itemAboutToBeAdded(Buddy buddy)
{
	buddy.setAnonymous(false);

	emit buddyAboutToBeAdded(buddy);

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

void BuddyManager::itemAdded(Buddy buddy)
{
	emit buddyAdded(buddy);
}

void BuddyManager::itemAboutToBeRemoved(Buddy buddy)
{
	buddy.setAnonymous(true);

	emit buddyAboutToBeRemoved(buddy);
}

void BuddyManager::itemRemoved(Buddy buddy)
{
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

// TODO: 0.6.6
// 	if (BuddyRemovePredicateObject::inquireAll(buddy))
// 	{
// 		Buddies.removeAll(buddy);
// 		buddy.removeFromStorage();
// 	}

	emit buddyRemoved(buddy);
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	ensureLoaded();

	foreach (const Contact &contact, source.contacts())
		contact.setOwnerBuddy(destination);

	source.setAnonymous(true);
	removeItem(source);

	source.data()->setUuid(destination.uuid()); // just for case
// 	source.data() setData(destination.data()); // TODO: 0.6.6 tricky merge, this should work well ;)
	
	ConfigurationManager::instance()->flush();
}

Buddy BuddyManager::byId(Account account, const QString &id)
{
	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Buddy::null;

	foreach (Buddy buddy, items())
	{
		if (id == buddy.id(account))
			return buddy;
	}

	Buddy anonymous = account.createAnonymous(id);
	addItem(anonymous);

	return anonymous;
}

Buddy BuddyManager::byDisplay(const QString &display)
{
	ensureLoaded();

	if (display.isEmpty())
		return Buddy::null;

	foreach (Buddy buddy, items())
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

BuddyList BuddyManager::buddies(Account account, bool includeAnonymous)
{
	ensureLoaded();

	BuddyList result;

	foreach (Buddy buddy, items())
		if (buddy.hasContact(account) && (includeAnonymous || !buddy.isAnonymous()))
			result << buddy;

	return result;
}

const Buddy & BuddyManager::byBuddyShared(BuddyShared *data)
{
	ensureLoaded();

	foreach (const Buddy &buddy, items())
		if (data == buddy.data())
			return buddy;

	return Buddy::null;
}

void BuddyManager::buddyDataUpdated()
{
	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyUpdated(buddy);
}

void BuddyManager::contactAboutToBeAdded(Contact contact)
{
	Buddy buddy(sender());
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
	foreach (Buddy buddy, items())
		buddy.removeFromGroup(group);
}
