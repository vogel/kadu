/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

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
	// QDomElement contactsNewNode = configurationStorage->getNode("ContactsNew", XmlConfigFile::ModeFind);
	// if (contactsNewNode.isNull())
	// {
		importConfiguration(configurationStorage);
		return;
	// }
}

void ContactManager::storeConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNewNode = configurationStorage->getNode("ContactsNew", XmlConfigFile::ModeCreate);

	foreach (Contact contact, Contacts.values())
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		QDomElement contactNode = configurationStorage->getUuidNode(contactsNewNode, "Contact", 
				contact.uuid(), XmlConfigFile::ModeCreate);
		contact.storeConfiguration(configurationStorage, contactNode);
	}
}

void ContactManager::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	emit contactAboutToBeAdded(contact);
	Contacts.insert(contact.uuid(), contact);
	emit contactAdded(contact);
}

Contact ContactManager::byId(Account *account, const QString &id)
{
	if (id.isEmpty() || 0 == account)
		return Contact::null;

	foreach (Contact contact, Contacts.values())
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

	foreach (Contact contact, Contacts.values())
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

	foreach (Contact contact, Contacts.values())
	{
		if (display == contact.display())
			return contact;
	}

	return Contact::null;
}

ContactList ContactManager::contacts(Account *account, bool includeAnonymous) const
{
	ContactList result;

	foreach (Contact contact, Contacts.values())
		if (contact.accountData(account) && (includeAnonymous || !contact.isAnonymous()))
			result << contact;

	return result;
}
