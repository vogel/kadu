/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
	QMutexLocker(&mutex());

	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupRemoved(Group)));

	int itemsSize = items().size();
	QDomElement buddiesNode = xml_config_file->getNode("Buddies", XmlConfigFile::ModeFind);
	QDomElement oldContactsNode = xml_config_file->getNode("OldContacts", XmlConfigFile::ModeFind);
	if (oldContactsNode.isNull() && (buddiesNode.isNull() || (itemsSize == 0 && !buddiesNode.hasAttribute("imported"))))
	{
		importConfiguration(xml_config_file);
		buddiesNode.setAttribute("imported", "true");
	}
}

void BuddyManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QMutexLocker(&mutex());

	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	contactsNode.setTagName("OldContacts");
	QList<QDomElement> contactElements = configurationStorage->getNodes(contactsNode, "Contact");
	foreach (const QDomElement &contactElement, contactElements)
	{
		Buddy buddy = Buddy::create();
		buddy.importConfiguration(contactElement);

		addItem(buddy);
	}

	// OldContacts is no longer needed
	contactsNode.parentNode().removeChild(contactsNode);

	// flush configuration to save all changes
	ConfigurationManager::instance()->flush();
}

void BuddyManager::load()
{
	QMutexLocker(&mutex());

	SimpleManager<Buddy>::load();
}

void BuddyManager::itemAboutToBeAdded(Buddy buddy)
{
	QMutexLocker(&mutex());

	connect(buddy, SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	connect(buddy, SIGNAL(buddySubscriptionChanged()), this, SLOT(buddySubscriptionChanged()));
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
	QMutexLocker(&mutex());

	disconnect(buddy, SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	disconnect(buddy, SIGNAL(buddySubscriptionChanged()), this, SLOT(buddySubscriptionChanged()));
	emit buddyRemoved(buddy);
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	QMutexLocker(&mutex());

	if (destination == source)
		return;

	ensureLoaded();

	foreach (const Contact &contact, source.contacts())
		contact.setOwnerBuddy(destination);

	source.setAnonymous(true);
	removeItem(source);
	// each item that stores pointer to "source" will now use the same uuid as "destination"
	source.data()->setUuid(destination.uuid());

	ConfigurationManager::instance()->flush();
}

Buddy BuddyManager::byDisplay(const QString &display, NotFoundAction action)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	if (display.isEmpty())
		return Buddy::null;

	foreach (const Buddy &buddy, items())
	{
		if (display == buddy.display())
			return buddy;
	}

	if (ActionReturnNull == action)
		return Buddy::null;

	Buddy buddy = Buddy::create();
	buddy.setDisplay(display);

	if (ActionCreateAndAdd == action)
		addItem(buddy);

	return buddy;
}

Buddy BuddyManager::byId(Account account, const QString &id, NotFoundAction action)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	Contact contact = ContactManager::instance()->byId(account, id, action);
	if (contact.isNull())
		return Buddy::null;

	return byContact(contact, action);
}

Buddy BuddyManager::byContact(Contact contact, NotFoundAction action)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	if (contact.isNull())
		return Buddy::null;

	if (ActionReturnNull == action || !contact.ownerBuddy().isAnonymous())
		return contact.ownerBuddy();

	if (!contact.ownerBuddy())
		contact.setOwnerBuddy(Buddy::create());

	if (ActionCreateAndAdd == action)
		addItem(contact.ownerBuddy());

	return contact.ownerBuddy();
}

Buddy BuddyManager::byUuid(const QUuid &uuid)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	if (uuid.isNull())
		return Buddy::create();

	foreach (const Buddy &buddy, items())
		if (buddy.uuid() == uuid)
			return buddy;

	return Buddy::create();
}

void BuddyManager::clearOwnerAndRemoveEmptyBuddy(Contact contact)
{
	if (!contact)
		return;

	Buddy owner = contact.ownerBuddy();
	contact.setOwnerBuddy(Buddy::null);

	if (owner && owner.isEmpty())
		removeItem(owner);
}

BuddyList BuddyManager::buddies(Account account, bool includeAnonymous)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	BuddyList result;

	foreach (const Buddy &buddy, items())
		if (buddy.hasContact(account) && (includeAnonymous || !buddy.isAnonymous()))
			result << buddy;

	return result;
}

void BuddyManager::buddyDataUpdated()
{
	QMutexLocker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyUpdated(buddy);
}

void BuddyManager::buddySubscriptionChanged()
{
	QMutexLocker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddySubscriptionChanged(buddy);
}

void BuddyManager::groupRemoved(Group group)
{
	QMutexLocker(&mutex());

	foreach (Buddy buddy, items())
		buddy.removeFromGroup(group);
}
