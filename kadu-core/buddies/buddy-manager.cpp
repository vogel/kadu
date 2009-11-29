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

		addItem(buddy);
	}
}

void BuddyManager::load()
{
	if (!needsLoad())
		return;

	if (xml_config_file->getNode("Buddies", XmlConfigFile::ModeFind).isNull())
	{
		importConfiguration(xml_config_file);
		setState(StateLoaded);
		return;
	}

	SimpleManager<Buddy>::load();
}


void BuddyManager::itemAboutToBeAdded(Buddy buddy)
{
	buddy.setAnonymous(false);

	emit buddyAboutToBeAdded(buddy);

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
		if (!buddy.contact(account).isNull() && (includeAnonymous || !buddy.isAnonymous()))
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

void BuddyManager::contactAboutToBeAdded(Account account)
{
	Buddy buddy(sender());
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
	foreach (Buddy buddy, items())
		buddy.removeFromGroup(group);
}
