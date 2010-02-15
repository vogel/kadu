/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Kermit (plaza.maciej@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "configuration/xml-configuration-file.h"
#include "storage/storable-object.h"

#include "configuration-manager.h"

ConfigurationManager * ConfigurationManager::Instance = 0;

ConfigurationManager * ConfigurationManager::instance()
{
	if (!Instance)
	{
		Instance = new ConfigurationManager();
		Instance->load();
	}

	return Instance;
}

ConfigurationManager::ConfigurationManager()
{

}

void ConfigurationManager::load()
{
	xml_config_file->makeBackup();

	importConfiguration();

	Uuid = xml_config_file->rootElement().attribute("uuid");
	if (Uuid.isNull())
		Uuid = QUuid::createUuid();
}

void ConfigurationManager::store()
{
	foreach (StorableObject *object, RegisteredStorableObjects)
		object->store();

	xml_config_file->rootElement().setAttribute("uuid", Uuid.toString());
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
	object->store();

	RegisteredStorableObjects.removeAll(object);
}

void ConfigurationManager::importConfiguration()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement general = xml_config_file->findElementByProperty(root.firstChild().firstChild().toElement(), "Group", "name", "General");
	QDomElement mainConfiguration = xml_config_file->findElementByProperty(general, "Entry", "name", "ConfigGeometry");
	
	if (root.elementsByTagName("Contacts").count() == 1 &&
		root.elementsByTagName("ContactsNew").count() == 0 &&
		root.elementsByTagName("Buddies").count() == 0 &&
		root.elementsByTagName("ContactAccountDatas").count() == 0)
		copyOldContactsToImport();

	if (root.elementsByTagName("Contacts").count() == 0 &&
	    root.elementsByTagName("ContactsNew").count() == 1)
		importOldContact();

	if (root.elementsByTagName("ContactsNew").count() == 1 &&
	    root.elementsByTagName("Buddies").count() == 0)
		importContactsIntoBuddies();
	
	if (root.elementsByTagName("ContactAccountDatas").count() == 1)
		importContactAccountDatasIntoContacts();
	
	if(!mainConfiguration.isNull())
		  mainConfiguration.setAttribute("name", "MainConfiguration_Geometry");

	
}

void ConfigurationManager::copyOldContactsToImport()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement oldContactsNode = root.elementsByTagName("Contacts").at(0).toElement();
	oldContactsNode.setTagName("OldContacts");

	flush();
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

void ConfigurationManager::importContactAccountDatasIntoContacts()
{
	QDomElement root = xml_config_file->rootElement();

	if (root.elementsByTagName("Contacts").size() > 0)
	{
		QDomElement oldContactsNode = root.elementsByTagName("Contacts").at(0).toElement();
		oldContactsNode.setTagName("OldContacts");
	}

	QDomElement contactsNode = root.elementsByTagName("ContactAccountDatas").at(0).toElement();
	contactsNode.setTagName("Contacts");
	
	QDomNodeList contacts = contactsNode.elementsByTagName("ContactAccountData");
	int count = contacts.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contact = contacts.at(i).toElement();
		if (contact.isNull())
			continue;
		
		contact.setTagName("Contact");
	}

	flush();
}
