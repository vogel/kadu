/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomElement>

#include "xml_config_file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

#include "pending_msgs.h"

#include "contacts/contact-list-configuration-helper.h"

ContactList ContactListConfigurationHelper::loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode)
{
	ContactList result;
	
	QDomNodeList contactNodes = configurationStorage->getNodes(contactListNode, "Contact");
	int count = contactNodes.count();

	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;
		result.append(ContactManager::instance()->byUuid(contactElement.text()));
	}
	
	return result;
}

void ContactListConfigurationHelper::saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode, ContactList contactList)
{
	foreach (Contact c, contactList)
		configurationStorage->createTextNode(contactListNode, "Contact", c.uuid());
}
