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

#include "buddy-set-configuration-helper.h"

BuddySet BuddySetConfigurationHelper::loadFromConfiguration(StorableObject *parent, const QString &nodeName)
{
	if (!parent->isValidStorage())
		return BuddySet();

	XmlConfigFile *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	return loadFromConfiguration(configurationStorage, contactSetNode);
}

BuddySet BuddySetConfigurationHelper::loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode)
{
	BuddySet result;

	QList<QDomElement> contactElements = configurationStorage->getNodes(contactSetNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
		result.insert(BuddyManager::instance()->byUuid(contactElement.text()));

	return result;
}

void BuddySetConfigurationHelper::saveToConfiguration(StorableObject *parent, const QString &nodeName, BuddySet contactSet)
{
	if (!parent->isValidStorage())
		return;

	XmlConfigFile *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	saveToConfiguration(configurationStorage, contactSetNode, contactSet);
}

void BuddySetConfigurationHelper::saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, BuddySet contactSet)
{
	while (contactSetNode.childNodes().count())
		contactSetNode.removeChild(contactSetNode.childNodes().at(0));

	foreach (Buddy c, contactSet)
		configurationStorage->appendTextNode(contactSetNode, "Contact", c.uuid());
}
