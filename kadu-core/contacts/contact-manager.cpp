/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "configuration/storage-point.h"

#include "contact.h"
#include "contact-list.h"
#include "xml_config_file.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager *  ContactManager::instance()
{
	if (0 == Instance)
		Instance = new ContactManager();

	return Instance;
}

StoragePoint * ContactManager::createStoragePoint() const
{
	QDomElement contactsNewNode = xml_config_file->getNode("ContactsNew");
	return new StoragePoint(xml_config_file, contactsNewNode);
}

void ContactManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QDomNodeList contactsNodes = configurationStorage->getNodes(contactsNode, "Contact");
	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactsNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;

		Contact contact;
		contact.importConfiguration(configurationStorage, contactElement);

		addContact(contact);
	}
}

void ContactManager::loadConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNewNode = configurationStorage->getNode("ContactsNew", XmlConfigFile::ModeFind);
	if (contactsNewNode.isNull())
	{
		importConfiguration(configurationStorage);
		return;
	}

	QDomNodeList contactsNodes = contactsNewNode.elementsByTagName("Contact");

	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomNode contactNode = contactsNodes.at(i);
		QDomElement contactElement = contactNode.toElement();
		if (contactElement.isNull())
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, contactElement);
		addContact(Contact::loadFromStorage(contactStoragePoint));
	}
}

void ContactManager::storeConfiguration(XmlConfigFile *configurationStorage)
{
	foreach (Contact contact, Contacts)
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		contact.storeConfiguration();
	}
}

void ContactManager::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	emit contactAboutToBeAdded(contact);
	Contacts.append(contact);
	emit contactAdded(contact);
}

Contact ContactManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return Contact::null;

	return Contacts.at(index);
}

Contact ContactManager::byId(Account *account, const QString &id)
{
	if (id.isEmpty() || 0 == account)
		return Contact::null;

	foreach (Contact contact, Contacts)
	{
		if (id == contact.id(account))
			return contact;
	}

	Contact anonymous = account->createAnonymous(id);
	addContact(anonymous);

	return anonymous;
}

Contact ContactManager::byUuid(const QString &uuid) const
{
	if (uuid.isEmpty())
		return Contact::null;

	foreach (Contact contact, Contacts)
	{
		if (uuid == contact.uuid().toString())
			return contact;
	}

	return Contact::null;
}

Contact ContactManager::byDisplay(const QString &display) const
{
	if (display.isEmpty())
		return Contact::null;

	foreach (Contact contact, Contacts)
	{
		if (display == contact.display())
			return contact;
	}

	return Contact::null;
}

ContactList ContactManager::contacts(Account *account, bool includeAnonymous) const
{
	ContactList result;

	foreach (Contact contact, Contacts)
		if (contact.accountData(account) && (includeAnonymous || !contact.isAnonymous()))
			result << contact;

	return result;
}
