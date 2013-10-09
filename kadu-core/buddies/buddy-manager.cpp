/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy-list.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "protocols/roster.h"
#include "storage/storage-point.h"

#include "debug.h"

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
	QMutexLocker locker(&mutex());

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
	QMutexLocker locker(&mutex());

	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	contactsNode.setTagName("OldContacts");
	QVector<QDomElement> contactElements = configurationStorage->getNodes(contactsNode, "Contact");
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
	QMutexLocker locker(&mutex());

	SimpleManager<Buddy>::load();
}

void BuddyManager::itemAboutToBeAdded(Buddy buddy)
{
	QMutexLocker locker(&mutex());

	connect(buddy, SIGNAL(updated()), this, SLOT(buddyDataUpdated()));
	connect(buddy, SIGNAL(buddySubscriptionChanged()), this, SLOT(buddySubscriptionChanged()));

	connect(buddy, SIGNAL(contactAboutToBeAdded(Contact)),
	        this, SLOT(buddyContactAboutToBeAdded(Contact)));
	connect(buddy, SIGNAL(contactAdded(Contact)),
	        this, SLOT(buddyContactAdded(Contact)));
	connect(buddy, SIGNAL(contactAboutToBeRemoved(Contact)),
	        this, SLOT(buddyContactAboutToBeRemoved(Contact)));
	connect(buddy, SIGNAL(contactRemoved(Contact)),
	        this, SLOT(buddyContactRemoved(Contact)));

	emit buddyAboutToBeAdded(buddy);
}

void BuddyManager::itemAdded(Buddy buddy)
{
	emit buddyAdded(buddy);
}

void BuddyManager::itemAboutToBeRemoved(Buddy buddy)
{
	foreach (const Contact &contact, buddy.contacts())
		contact.setOwnerBuddy(Buddy::null);

	emit buddyAboutToBeRemoved(buddy);
}

void BuddyManager::itemRemoved(Buddy buddy)
{
	QMutexLocker locker(&mutex());

	disconnect(buddy, 0, this, 0);

	emit buddyRemoved(buddy);
}

QString BuddyManager::mergeValue(const QString &destination, const QString &source) const
{
	if (destination.isEmpty())
		return source;
	else
		return destination;
}

void BuddyManager::mergeBuddies(Buddy destination, Buddy source)
{
	QMutexLocker locker(&mutex());

	if (destination == source)
		return;

	ensureLoaded();

	destination.setEmail(mergeValue(destination.email(), source.email()));
	destination.setHomePhone(mergeValue(destination.homePhone(), source.homePhone()));
	destination.setMobile(mergeValue(destination.mobile(), source.mobile()));
	destination.setWebsite(mergeValue(destination.website(), source.website()));

	// we need to move contacts before removing source buddy as this would cause
	// these contacts to detach and remove from roster
	// i think this is another reason why we should not automate too much
	// we should just manually delete all contacts when buddy is removed

	foreach (const Contact &contact, source.contacts())
		contact.setOwnerBuddy(destination);

	removeItem(source);

	source.setAnonymous(true);
	// each item that stores pointer to "source" will now use the same uuid as "destination"
	source.data()->setUuid(destination.uuid());

	ConfigurationManager::instance()->flush();
}

Buddy BuddyManager::byDisplay(const QString &display, NotFoundAction action)
{
	QMutexLocker locker(&mutex());

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
	QMutexLocker locker(&mutex());

	ensureLoaded();

	Contact contact = ContactManager::instance()->byId(account, id, action);
	if (contact.isNull())
		return Buddy::null;

	return byContact(contact, action);
}

Buddy BuddyManager::byContact(Contact contact, NotFoundAction action)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (!contact)
		return Buddy::null;

	if (ActionReturnNull == action || !contact.isAnonymous())
		return contact.ownerBuddy();

	if (!contact.ownerBuddy())
		contact.setOwnerBuddy(Buddy::create());

	if (ActionCreateAndAdd == action)
		addItem(contact.ownerBuddy());

	return contact.ownerBuddy();
}

Buddy BuddyManager::byUuid(const QUuid &uuid)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (uuid.isNull())
		return Buddy::create();

	foreach (const Buddy &buddy, items())
		if (buddy.uuid() == uuid)
			return buddy;

	return Buddy::create();
}

void BuddyManager::removeBuddyIfEmpty(Buddy buddy, bool checkOnlyForContacts)
{
	if (!buddy)
		return;

	if (buddy.isEmpty(checkOnlyForContacts))
		removeItem(buddy);
}

void BuddyManager::clearOwnerAndRemoveEmptyBuddy(Contact contact, bool checkBuddyOnlyForOtherContacts)
{
	if (!contact)
		return;

	Buddy owner = contact.ownerBuddy();
	contact.setOwnerBuddy(Buddy::null);
	removeBuddyIfEmpty(owner, checkBuddyOnlyForOtherContacts);
}

BuddyList BuddyManager::buddies(Account account, bool includeAnonymous)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	BuddyList result;

	foreach (const Buddy &buddy, items())
		if (buddy.hasContact(account) && (includeAnonymous || !buddy.isAnonymous()))
			result << buddy;

	return result;
}

void BuddyManager::buddyDataUpdated()
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyUpdated(buddy);
}

void BuddyManager::buddySubscriptionChanged()
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddySubscriptionChanged(buddy);
}

void BuddyManager::buddyContactAboutToBeAdded(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyContactAboutToBeAdded(buddy, contact);
}

void BuddyManager::buddyContactAdded(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyContactAdded(buddy, contact);
}

void BuddyManager::buddyContactAboutToBeRemoved(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyContactAboutToBeRemoved(buddy, contact);
}

void BuddyManager::buddyContactRemoved(const Contact &contact)
{
	QMutexLocker locker(&mutex());

	Buddy buddy(sender());
	if (!buddy.isNull())
		emit buddyContactRemoved(buddy, contact);
}

#include "moc_buddy-manager.cpp"
