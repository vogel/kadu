/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPair>
#include <QtCore/QTimer>

#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-parser-tags.h"
#include "core/core.h"
#include "message/unread-message-repository.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/services/roster/roster-entry.h"
#include "debug.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
	{
		// this must be called before ContactManager has its own pretty storage point
		// if ContactManager is called first and import from 0.6.5 is done
		// when ContactManager's storage() point will be modified from Contacts to OldContacts
		// and everything will be fucked up
		BuddyManager::instance();

		Instance = new ContactManager();
		Instance->init();
	}

	return Instance;
}

ContactManager::ContactManager()
{
}

ContactManager::~ContactManager()
{
	disconnect(Core::instance()->unreadMessageRepository(), 0, this, 0);

	foreach (const Message &message, Core::instance()->unreadMessageRepository()->allUnreadMessages())
		unreadMessageRemoved(message);

	ContactParserTags::unregisterParserTags();
}

void ContactManager::init()
{
	// needed for QueuedConnection
	qRegisterMetaType<Contact>("Contact");

	ContactParserTags::registerParserTags();

	foreach (const Message &message, Core::instance()->unreadMessageRepository()->allUnreadMessages())
		unreadMessageAdded(message);

	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ContactManager::dirtinessChanged()
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull() && contact.ownerBuddy() != Core::instance()->myself())
	{
		if (contact.rosterEntry()->requiresSynchronization())
		{
			DirtyContacts.append(contact);
			emit dirtyContactAdded(contact);
		}
		else
			DirtyContacts.removeAll(contact);
	}
}

void ContactManager::unreadMessageAdded(const Message &message)
{
	const Contact &contact = message.messageSender();
	contact.setUnreadMessagesCount(contact.unreadMessagesCount() + 1);
}

void ContactManager::unreadMessageRemoved(const Message &message)
{
	const Contact &contact = message.messageSender();
	quint16 unreadMessagesCount = contact.unreadMessagesCount();
	if (unreadMessagesCount > 0)
		contact.setUnreadMessagesCount(unreadMessagesCount - 1);
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
	QMutexLocker locker(&mutex());

	connect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeAdded(item);
}

void ContactManager::itemRegistered(Contact item)
{
	if (!item)
		return;

	QMutexLocker locker(&mutex());

	emit contactAdded(item);

	if (Core::instance()->myself() == item.ownerBuddy())
		item.rosterEntry()->setState(RosterEntrySynchronized);
	else if (item.rosterEntry()->requiresSynchronization())
	{
		DirtyContacts.append(item);
		emit dirtyContactAdded(item);
	}

	connect(item, SIGNAL(dirtinessChanged()), this, SLOT(dirtinessChanged()));
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	QMutexLocker locker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeRemoved(item);
}

void ContactManager::itemUnregistered(Contact item)
{
	disconnect(item, SIGNAL(dirtinessChanged()), this, SLOT(dirtinessChanged()));

	if (item && item.rosterEntry()->requiresSynchronization())
		DirtyContacts.removeAll(item);

	emit contactRemoved(item);
}

Contact ContactManager::byId(Account account, const QString &id, NotFoundAction action)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Contact::null;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount() && id == contact.id())
			return contact;

	if (action == ActionReturnNull)
		return Contact::null;

	Contact contact = Contact::create();
	contact.setId(id);
	contact.setContactAccount(account);

	if (action == ActionCreateAndAdd)
		addItem(contact);

	ContactDetails *details = contact.details();
	if (details)
		details->setState(StateNew);

	Buddy buddy = Buddy::create();
	contact.setOwnerBuddy(buddy);
	contact.rosterEntry()->setState(RosterEntrySynchronized);

	return contact;
}

QVector<Contact> ContactManager::contacts(Account account, AnonymousInclusion inclusion)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount() && ((IncludeAnonymous == inclusion) || !contact.isAnonymous()))
			contacts.append(contact);

	return contacts;
}

QVector<Contact> ContactManager::dirtyContacts(Account account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, DirtyContacts)
		if (account == contact.contactAccount())
			contacts.append(contact);

	return contacts;
}

void ContactManager::contactDataUpdated()
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactUpdated(contact);
}

// This is needed to fix up configurations broken by bug #2222 (present in 0.9.x).
// It can be removed when we will stop supporting upgrades from 0.9.x.
void ContactManager::removeDuplicateContacts()
{
	QMap<QPair<Account, QString>, Contact> uniqueContacts;

	foreach (const Contact &contact, allItems())
	{
		QMap<QPair<Account, QString>, Contact>::iterator it = uniqueContacts.find(qMakePair(contact.contactAccount(), contact.id()));
		if (it != uniqueContacts.end())
		{
			if (it->isAnonymous())
			{
				removeItem(*it);
				it->setUuid(contact.uuid());
				*it = contact;
			}
			else
			{
				removeItem(contact);
				contact.setUuid(it->uuid());
			}
		}
		else
			uniqueContacts.insert(qMakePair(contact.contactAccount(), contact.id()), contact);
	}

	config_file.writeEntry("General", "ContactsImportedFrom0_9", true);
}

void ContactManager::loaded()
{
	Manager<Contact>::loaded();

	if (!config_file.readBoolEntry("General", "ContactsImportedFrom0_9", false))
		// delay it so that everything needed will be loaded when we call this method
		QTimer::singleShot(0, this, SLOT(removeDuplicateContacts()));
}

#include "moc_contact-manager.cpp"
