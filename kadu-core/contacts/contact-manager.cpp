/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPair>
#include <QtCore/QTimer>

#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-parser-tags.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "debug.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
		Instance = new ContactManager();

	return Instance;
}

ContactManager::ContactManager()
{
	// needed for QueuedConnection
	qRegisterMetaType<Contact>("Contact");

	ContactParserTags::registerParserTags();
}

ContactManager::~ContactManager()
{
	ContactParserTags::unregisterParserTags();
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

void ContactManager::aboutToBeDetached(bool reattaching)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAboutToBeDetached(contact, reattaching);
}

void ContactManager::detached(Buddy previousBuddy)
{
	QMutexLocker locker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactDetached(contact, previousBuddy);
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
	connect(item, SIGNAL(aboutToBeDetached(bool)), this, SLOT(aboutToBeDetached(bool)));
	connect(item, SIGNAL(detached(Buddy)), this, SLOT(detached(Buddy)));
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
	disconnect(item, SIGNAL(aboutToBeDetached(bool)), this, SLOT(aboutToBeDetached(bool)));
	disconnect(item, SIGNAL(detached(Buddy)), this, SLOT(detached(Buddy)));
	disconnect(item, SIGNAL(aboutToBeAttached(Buddy)), this, SLOT(aboutToBeAttached(Buddy)));
	disconnect(item, SIGNAL(attached(bool)), this, SLOT(attached(bool)));

	if (item.isDirty())
		DirtyContacts.removeAll(item);

	emit contactRemoved(item);
}

void ContactManager::detailsLoaded(Contact item)
{
	QMutexLocker locker(&mutex());

	if (!item.isNull())
		registerItem(item);
}

void ContactManager::detailsUnloaded(Contact item)
{
	QMutexLocker locker(&mutex());

	if (!item.isNull())
		unregisterItem(item);
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

QList<Contact> ContactManager::contacts(Account account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QList<Contact> contacts;

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

QList<Contact> ContactManager::dirtyContacts(Account account)
{
	QMutexLocker locker(&mutex());

	ensureLoaded();

	QList<Contact> contacts;

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
			if (it->ownerBuddy().isAnonymous())
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
