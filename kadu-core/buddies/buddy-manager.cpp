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
#include "buddies/buddy-shared.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
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
	
	// flush configuration to save all changes
	ConfigurationManager::instance()->flush();
}

void BuddyManager::load()
{
	SimpleManager<Buddy>::load();
}

void BuddyManager::itemAboutToBeAdded(Buddy buddy)
{
	connect(buddy, SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	emit buddyAboutToBeAdded(buddy);
}

void BuddyManager::itemAdded(Buddy buddy)
{
	emit buddyAdded(buddy);
}

void BuddyManager::itemAboutToBeRemoved(Buddy buddy)
{
	emit buddyAboutToBeRemoved(buddy);
}

void BuddyManager::itemRemoved(Buddy buddy)
{
	disconnect(buddy, SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
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

Buddy BuddyManager::byId(Account account, const QString &id, bool create)
{
	Contact contact = ContactManager::instance()->byId(account, id, create);
	if (contact.isNull())
		return Buddy::null;

	return byContact(contact, create);
}

Buddy BuddyManager::byContact(Contact contact, bool create)
{
	if (contact.isNull())
		return Buddy::null;

	if (!create || !contact.ownerBuddy().isNull())
		return contact.ownerBuddy();

	Buddy buddy = Buddy::create();
	buddy.setDisplay(QString("%1: %2").arg(contact.contactAccount().name()).arg(contact.id()));
	addItem(buddy);

	return buddy;
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

void BuddyManager::buddyDataUpdated()
{
	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyUpdated(buddy);
}

void BuddyManager::groupRemoved(Group group)
{
	foreach (Buddy buddy, items())
		buddy.removeFromGroup(group);
}
