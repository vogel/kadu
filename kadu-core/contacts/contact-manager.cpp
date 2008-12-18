/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"
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

		Contact *contact = new Contact();
		contact->importConfiguration(configurationStorage, contactElement);

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

	foreach (Contact *contact, Contacts.values())
	{
		QDomElement contactNode = configurationStorage->getUuidNode(contactsNewNode, "Contact", contact->uuid(), XmlConfigFile::ModeCreate);
		contact->storeConfiguration(configurationStorage, contactNode);
	}
}

void ContactManager::addContact(Contact *contact)
{
	Contacts.insert(contact->uuid(), contact);

	emit contactAdded(contact);
}
