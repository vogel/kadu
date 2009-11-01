/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomElement>

#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"

#include "buddies/buddy-list-configuration-helper.h"

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
	while (contactListNode.childNodes().count())
		contactListNode.removeChild(contactListNode.childNodes().at(0));

	foreach (Contact c, contactList)
		configurationStorage->appendTextNode(contactListNode, "Contact", c.uuid());
}
