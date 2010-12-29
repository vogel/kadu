/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"
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
	ContactParserTags::registerParserTags();
}

ContactManager::~ContactManager()
{
	ContactParserTags::unregisterParserTags();
}

void ContactManager::idChanged(const QString &oldId)
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactIdChanged(contact, oldId);
}

void ContactManager::aboutToBeDetached()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAboutToBeDetached(contact);
}

void ContactManager::detached()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactDetached(contact);
}

void ContactManager::aboutToBeAttached()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAboutToBeAttached(contact);
}

void ContactManager::attached()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactAttached(contact);
}

void ContactManager::reattached()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactReattached(contact);
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
	QMutexLocker(&mutex());

	connect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeAdded(item);
}

void ContactManager::itemRegistered(Contact item)
{
	QMutexLocker(&mutex());

	emit contactAdded(item);

	connect(item, SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
	connect(item, SIGNAL(aboutToBeDetached()), this, SLOT(aboutToBeDetached()));
	connect(item, SIGNAL(detached()), this, SLOT(detached()));
	connect(item, SIGNAL(aboutToBeAttached()), this, SLOT(aboutToBeAttached()));
	connect(item, SIGNAL(attached()), this, SLOT(attached()));
	connect(item, SIGNAL(reattached()), this, SLOT(reattached()));
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	QMutexLocker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAboutToBeRemoved(item);
}

void ContactManager::itemUnregistered(Contact item)
{
	disconnect(item, SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
	disconnect(item, SIGNAL(aboutToBeDetached()), this, SLOT(aboutToBeDetached()));
	disconnect(item, SIGNAL(detached()), this, SLOT(detached()));
	disconnect(item, SIGNAL(aboutToBeAttached()), this, SLOT(aboutToBeAttached()));
	disconnect(item, SIGNAL(attached()), this, SLOT(attached()));
	disconnect(item, SIGNAL(reattached()), this, SLOT(reattached()));

	emit contactRemoved(item);
}

void ContactManager::detailsLoaded(Contact item)
{
	QMutexLocker(&mutex());

	if (!item.isNull())
		registerItem(item);
}

void ContactManager::detailsUnloaded(Contact item)
{
	QMutexLocker(&mutex());

	if (!item.isNull())
		unregisterItem(item);
}

Contact ContactManager::byId(Account account, const QString &id, NotFoundAction action)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Contact::null;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount() && id == contact.id())
			return contact;

	if (action == ActionReturnNull)
		return Contact::null;

	Contact contact = Contact::create();
	contact.setContactAccount(account);
	contact.setId(id);

	if (action == ActionCreateAndAdd)
		addItem(contact);

	Protocol *protocolHandler = account.protocolHandler();
	if (!protocolHandler)
		return contact;

	ProtocolFactory *factory = protocolHandler->protocolFactory();
	if (!factory)
		return contact;

	ContactDetails *details = factory->createContactDetails(contact);
	details->setState(StateNew);
	contact.setDetails(details);

	Buddy buddy = Buddy::create();
	contact.setOwnerBuddy(buddy);
	
	return contact;
}

QList<Contact> ContactManager::contacts(Account account)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount())
			contacts.append(contact);

	return contacts;
}

void ContactManager::contactDataUpdated()
{
	QMutexLocker(&mutex());

	Contact contact(sender());
	if (!contact.isNull())
		emit contactUpdated(contact);
}
