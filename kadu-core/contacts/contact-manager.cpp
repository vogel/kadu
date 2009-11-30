/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "contacts/contact.h"
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
}

ContactManager::~ContactManager()
{
}

void ContactManager::itemAdded(Contact item)
{
	connect(item, SIGNAL(protocolLoaded()), this, SLOT(contactProtocolLoaded()));
	connect(item, SIGNAL(protocolUnloaded()), this, SLOT(contactProtocolUnloaded()));
}

void ContactManager::itemRemoved(Contact item)
{
	disconnect(item, SIGNAL(protocolLoaded()), this, SLOT(contactProtocolLoaded()));
	disconnect(item, SIGNAL(protocolUnloaded()), this, SLOT(contactProtocolUnloaded()));
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
	emit contactAboutToBeAdded(item);
}

void ContactManager::itemRegisterd(Contact item)
{
	emit contactAdded(item);
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	emit contactAboutToBeRemoved(item);
}

void ContactManager::itemUnregistered(Contact item)
{
	emit contactRemoved(item);
}

void ContactManager::detailsLoaded(Contact item)
{
	if (!item.isNull())
		registerItem(item);
}

void ContactManager::detailsUnloaded(Contact item)
{
	if (!item.isNull())
		unregisterItem(item);
}

Contact ContactManager::byId(Account account, const QString &id)
{
	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Contact::null;

	foreach (const Contact &contact, contacts(account))
		if (id == contact.id())
			return contact;

	return Contact::null;
}

QList<Contact> ContactManager::contacts(Account account)
{
	ensureLoaded();

	QList<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount())
			contacts.append(contact);

	return contacts;
}
