/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/storable-object.h"
#include "configuration/xml-configuration-file.h"

#include "configuration-manager.h"

ConfigurationManager::ConfigurationManager()
{

}

void ConfigurationManager::load()
{
	xml_config_file->makeBackup();

	importConfiguration();
}

void ConfigurationManager::store()
{
	foreach (StorableObject *object, RegisteredStorableObjects)
		object->store();
}

void ConfigurationManager::flush()
{
	store();
	xml_config_file->sync();
}

void ConfigurationManager::registerStorableObject(StorableObject *object)
{
	RegisteredStorableObjects.append(object);
}

void ConfigurationManager::unregisterStorableObject(StorableObject *object)
{
	RegisteredStorableObjects.removeAll(object);
}

void ConfigurationManager::importConfiguration()
{
	QDomElement root = xml_config_file->rootElement();

	if (root.elementsByTagName("Contacts").count() == 0 &&
	    root.elementsByTagName("ContactsNew").count() == 1)
		importOldContact();

	if (root.elementsByTagName("ContactsNew").count() == 1 &&
	    root.elementsByTagName("Buddies").count() == 0)
		importContactsIntoBuddies();
}

void ConfigurationManager::importOldContact()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement contactsNew = root.elementsByTagName("ContactsNew").at(0).toElement();
	QDomElement contactsNode = xml_config_file->createElement(root, "Contacts");

	QDomNodeList contacts = contactsNew.elementsByTagName("Contact");
	int count = contacts.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contact = contacts.at(i).toElement();
		if (contact.isNull())
			continue;

		QDomNodeList contacts = contact.elementsByTagName("Contact");
		int datasCount = contacts.size();
		for (int j = 0; j < datasCount; j++)
		{
			QDomElement contact = contacts.at(j).toElement();
			if (contact.isNull())
				continue;

			contact.removeChild(contact);
			contactsNode.appendChild(contact);
			xml_config_file->createTextNode(contact, "Contact", contact.attribute("uuid"));
		}
	}

	flush();
}

void ConfigurationManager::importContactsIntoBuddies()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement buddiesNode = root.elementsByTagName("ContactsNew").at(0).toElement();
	buddiesNode.setTagName("Buddies");

	QDomNodeList buddies = buddiesNode.elementsByTagName("Contact");
	int count = buddies.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement buddy = buddies.at(i).toElement();
		if (buddy.isNull())
			continue;

		buddy.setTagName("Buddy");
	}

	flush();
}
