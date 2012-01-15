/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact-parser-tags.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "message/message-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
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
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	           this, SLOT(unreadMessageAdded(Message)));
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	           this, SLOT(unreadMessageRemoved(Message)));

	foreach (const Message &message, MessageManager::instance()->allUnreadMessages())
		unreadMessageRemoved(message);

	ContactParserTags::unregisterParserTags();
}

void ContactManager::init()
{
	// needed for QueuedConnection
	qRegisterMetaType<Contact>("Contact");

	ContactParserTags::registerParserTags();

	foreach (const Message &message, MessageManager::instance()->allUnreadMessages())
		unreadMessageAdded(message);

	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void ContactManager::idChanged(const QString &oldId)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactIdChanged(contact, oldId);
}

void ContactManager::dirtinessChanged()
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull() && contact.ownerBuddy() != Core::instance()->myself())
	{
		if (contact.isDirty())
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

void ContactManager::aboutToBeDetached()
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAboutToBeDetached(contact);
}

void ContactManager::detached(Buddy previousBuddy, bool reattaching)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactDetached(contact, previousBuddy, reattaching);
}

void ContactManager::aboutToBeAttached(Buddy nearFutureBuddy)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAboutToBeAttached(contact, nearFutureBuddy);
}

void ContactManager::attached(bool reattached)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAttached(contact, reattached);
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
	QMutexLocker locker(&mutex());

	connect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeAdded(item);
}

void ContactManager::itemRegistered(Contact item)
{
	QMutexLocker locker(&mutex());

	emit contactAdded(item);

	if (Core::instance()->myself() == item.ownerBuddy())
		item.setDirty(false);
	else if (item.isDirty())
	{
		DirtyContacts.append(item);
		emit dirtyContactAdded(item);
	}

	connect(item, SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
	connect(item, SIGNAL(dirtinessChanged()), this, SLOT(dirtinessChanged()));
	connect(item, SIGNAL(aboutToBeDetached()), this, SLOT(aboutToBeDetached()));
	connect(item, SIGNAL(detached(Buddy, bool)), this, SLOT(detached(Buddy, bool)));
	connect(item, SIGNAL(aboutToBeAttached(Buddy)), this, SLOT(aboutToBeAttached(Buddy)));
	connect(item, SIGNAL(attached(bool)), this, SLOT(attached(bool)));
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	QMutexLocker locker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeRemoved(item);
}

void ContactManager::itemUnregistered(Contact item)
{
	disconnect(item, SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
	disconnect(item, SIGNAL(dirtinessChanged()), this, SLOT(dirtinessChanged()));
	disconnect(item, SIGNAL(aboutToBeDetached()), this, SLOT(aboutToBeDetached()));
	disconnect(item, SIGNAL(detached(Buddy, bool)), this, SLOT(detached(Buddy, bool)));
	disconnect(item, SIGNAL(aboutToBeAttached(Buddy)), this, SLOT(aboutToBeAttached(Buddy)));
	disconnect(item, SIGNAL(attached(bool)), this, SLOT(attached(bool)));

	if (item.isDirty())
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
	contact.setDirty(false);

	return contact;
}

QVector<Contact> ContactManager::contacts(Account account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QVector<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount())
			contacts.append(contact);

	return contacts;
}

const QList<Contact> & ContactManager::dirtyContacts()
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	return DirtyContacts;
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
