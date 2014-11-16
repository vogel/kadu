/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-parser-tags.h"
#include "core/application.h"
#include "core/core.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier-lock.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "roster/roster-entry.h"
#include "roster/roster-entry-state.h"
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

		// moved from here because of #2758
		// Instance->init();
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
		item.rosterEntry()->setSynchronized();
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	QMutexLocker locker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeRemoved(item);
}

void ContactManager::itemUnregistered(Contact item)
{
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

	ChangeNotifierLock lock(contact.rosterEntry()->hasLocalChangesNotifier(), ChangeNotifierLock::ModeForget); // don't emit dirty signals
	contact.setId(id);
	contact.setContactAccount(account);

	if (action == ActionCreateAndAdd)
		addItem(contact);

	ContactDetails *details = contact.details();
	if (details)
		details->setState(StateNew);

	Buddy buddy = Buddy::create();
	contact.setOwnerBuddy(buddy);

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

	Application::instance()->configuration()->deprecatedApi()->writeEntry("General", "ContactsImportedFrom0_9", true);
}

void ContactManager::loaded()
{
	Manager<Contact>::loaded();

	if (!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ContactsImportedFrom0_9", false))
		// delay it so that everything needed will be loaded when we call this method
		QTimer::singleShot(0, this, SLOT(removeDuplicateContacts()));
}

#include "moc_contact-manager.cpp"
