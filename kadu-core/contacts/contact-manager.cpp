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
	ConfigurationManager::instance()->registerStorableObject(this);
}

ContactManager::~ContactManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

StoragePoint * ContactManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Contacts"));
}

void ContactManager::load()
{
	if (!isValidStorage())
		return;

	if (!needsLoad())
		return;

	StorableObject::load();

	QDomElement contactsNode = storage()->point();
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactElements = storage()->storage()->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
	{
		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), contactElement);
		Contact contact = Contact::loadFromStorage(storagePoint);

		AllContacts.append(contact);

		connect(contact, SIGNAL(protocolLoaded()), this, SLOT(contactProtocolLoaded()));
		connect(contact, SIGNAL(protocolUnloaded()), this, SLOT(contactProtocolUnloaded()));

		if (contact.contactAccount().protocolHandler())
			addContact(contact);
	}
}

void ContactManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::ensureLoaded();

	foreach (Contact contact, AllContacts)
		if (!contact.isNull())
			contact.store();
}

void ContactManager::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	StorableObject::ensureLoaded();

	if (LoadedContacts.contains(contact))
		return;

	emit contactAboutToBeAdded(contact);
	LoadedContacts.append(contact);
	emit contactAdded(contact);
}

void ContactManager::removeContact(Contact contact)
{
	kdebugf();

	if (contact.isNull())
		return;

	ensureLoaded();

	if (!LoadedContacts.contains(contact))
		return;

	emit contactAboutToBeRemoved(contact);
	LoadedContacts.removeAll(contact);
	emit contactRemoved(contact);
}

Contact ContactManager::byIndex(unsigned int index)
{
	ensureLoaded();

	if (index < 0 || index >= count())
		return Contact::null;

	return LoadedContacts.at(index);
}

Contact ContactManager::byUuid(const QString &uuid)
{
	ensureLoaded();

	if (uuid.isEmpty())
		return Contact::null;

	foreach (Contact contact, AllContacts)
		if (!contact.isNull())
			if (uuid == contact.uuid().toString())
				return contact;

	return Contact::null;
}

Contact ContactManager::byContactShared(ContactShared *data)
{
	ensureLoaded();

	foreach (Contact contact, AllContacts)
		if (data == contact.data())
			return contact;

	return data;
}

void ContactManager::contactProtocolLoaded()
{
	Contact contact(sender());
	if (!contact.isNull())
		addContact(contact);
}

void ContactManager::contactProtocolUnloaded()
{
	Contact contact(sender());
	if (!contact.isNull())
		removeContact(contact);
}
