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

	if (root.elementsByTagName("ContactAccountDatas").count() == 0 &&
	    root.elementsByTagName("ContactsNew").count() == 1)
		importOldContactAccountData();
}

void ConfigurationManager::importOldContactAccountData()
{
	QDomElement root = xml_config_file->rootElement();
	QDomElement contactsNew = root.elementsByTagName("ContactsNew").at(0).toElement();
	QDomElement contactAccountDatasNode = xml_config_file->createElement(root, "ContactAccountDatas");

	QDomNodeList contacts = contactsNew.elementsByTagName("Contact");
	int count = contacts.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contact = contacts.at(i).toElement();
		if (contact.isNull())
			continue;

		QDomNodeList contactAccountDatas = contact.elementsByTagName("ContactAccountData");
		int datasCount = contactAccountDatas.size();
		for (int j = 0; j < datasCount; j++)
		{
			QDomElement contactAccountData = contactAccountDatas.at(j).toElement();
			if (contactAccountData.isNull())
				continue;

			contact.removeChild(contactAccountData);
			contactAccountDatasNode.appendChild(contactAccountData);
			xml_config_file->createTextNode(contactAccountData, "Contact", contact.attribute("uuid"));
		}
	}

	flush();
}
