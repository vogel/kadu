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
#include "core/core.h"
#include "debug.h"

#include "contact-manager.h"
#include <protocols/protocol.h>
#include <protocols/protocol-factory.h>

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ContactManager();
		Instance->init();
	}

	return Instance;
}

ContactManager::ContactManager()
{
	Core::instance()->configuration()->registerStorableObject(this);
}

ContactManager::~ContactManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);

	triggerAllAccountsUnregistered();
}

void ContactManager::init()
{
	triggerAllAccountsRegistered();
}

void ContactManager::loadContact(Contact contact)
{
	contact.loadDetails();
	addContact(contact);
}

void ContactManager::unloadContact(Contact contact)
{
	removeContact(contact);
	contact.unloadDetails();
}

void ContactManager::tryLoadContact(Contact contact)
{
	if (LoadedAccounts.contains(contact.contactAccount()))
		loadContact(contact);
}

StoragePoint * ContactManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Contacts"));
}
/*
void ContactManager::load(Account account)
{
	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement contactsNode = storage()->point();

	if (contactsNode.isNull())
		return;*/

	// TODO 0.6.6: by tag does not work, this works only if childNodes are "Chat"
// 	QDomNodeList contactsNodes = contactsNode.childNodes();
/*
	int count = contactsNodes.count();
	QString uuid = account.uuid().toString();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactsNodes.at(i).toElement();
		if (contactElement.isNull())
			continue;

		if (configurationStorage->getTextNode(contactElement, "Account") != uuid)
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, contactElement);
		if (!account.protocolHandler())
			return;

		Contact contact = account.protocolHandler()->protocolFactory()->loadContact(contactStoragePoint);

		if (cad)
		{
			addContact(cad);
			cad->data()->ensureLoaded();
		}
	}
}*/

void ContactManager::accountRegistered(Account account)
{
	printf("Account registered %d\n", AllContacts.count());
	if (LoadedAccounts.contains(account))
		return;

	LoadedAccounts.append(account);
	foreach (Contact contact, AllContacts)
		if (contact.contactAccount() == account)
			loadContact(contact);
}

void ContactManager::accountUnregistered(Account account)
{
	if (!LoadedAccounts.contains(account))
		return;

	LoadedAccounts.removeAll(account);
	foreach (Contact contact, LoadedContacts)
		unloadContact(contact);
}

void ContactManager::ensureLoaded(Account account)
{
	accountRegistered(account);
}

void ContactManager::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	QDomElement contactsNode = storage()->point();
	if (contactsNode.isNull())
		return;

	QList<QDomElement> contactElements = storage()->storage()->getNodes(contactsNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
	{
		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), contactElement);
		Contact contact = Contact::loadFromStorage(storagePoint);
		AllContacts.append(contact);

		tryLoadContact(contact);
	}
}

void ContactManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::ensureLoaded();

	foreach (Contact contact, AllContacts)
		contact.store();
}

void ContactManager::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	StorableObject::ensureLoaded();

	if (AllContacts.contains(contact))
		return;

	emit contactAboutToBeAdded(contact);
	AllContacts.append(contact);
	emit contactAdded(contact);
}

void ContactManager::removeContact(Contact contact)
{
	kdebugf();

	if (contact.isNull())
		return;

	StorableObject::ensureLoaded();

	if (!AllContacts.contains(contact))
		return;

	emit contactAboutToBeRemoved(contact);
	AllContacts.removeAll(contact);
	emit contactRemoved(contact);
}

Contact ContactManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return Contact::null;

	StorableObject::ensureLoaded();

	return LoadedContacts.at(index);
}

Contact ContactManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return Contact::null;

	StorableObject::ensureLoaded();

	foreach (Contact contact, AllContacts)
		if (uuid == contact.uuid().toString())
			return contact;

	return Contact::null;
}
