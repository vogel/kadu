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
#include "buddies/buddy-shared.h"
#include "buddies/buddy.h"

#include "buddies/buddy-list-configuration-helper.h"

BuddyList BuddyListConfigurationHelper::loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode)
{
	BuddyList result;
	
	QList<QDomElement> contactElements = configurationStorage->getNodes(contactListNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
		result.append(BuddyManager::instance()->byUuid(contactElement.text()));
	
	return result;
}

void BuddyListConfigurationHelper::saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode, BuddyList contactList)
{
	while (contactListNode.childNodes().count())
		contactListNode.removeChild(contactListNode.childNodes().at(0));

	foreach (Buddy c, contactList)
		configurationStorage->appendTextNode(contactListNode, "Contact", c.uuid());
}
