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
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

#include "contact-set-configuration-helper.h"

ContactSet ContactSetConfigurationHelper::loadFromConfiguration(StorableObject *parent, const QString &nodeName)
{
	if (!parent->isValidStorage())
		return ContactSet();

	XmlConfigFile *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	return loadFromConfiguration(configurationStorage, contactSetNode);
}

ContactSet ContactSetConfigurationHelper::loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode)
{
	ContactSet result;

	QDomNodeList contactNodes = configurationStorage->getNodes(contactSetNode, "Contact");
	int count = contactNodes.count();

	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;
		result.insert(ContactManager::instance()->byUuid(contactElement.text()));
	}

	return result;
}

void ContactSetConfigurationHelper::saveToConfiguration(StorableObject *parent, const QString &nodeName, ContactSet contactSet)
{
	if (!parent->isValidStorage())
		return;

	XmlConfigFile *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	saveToConfiguration(configurationStorage, contactSetNode, contactSet);
}

void ContactSetConfigurationHelper::saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, ContactSet contactSet)
{
	while (contactSetNode.childNodes().count())
		contactSetNode.removeChild(contactSetNode.childNodes().at(0));

	foreach (Contact c, contactSet)
		configurationStorage->appendTextNode(contactSetNode, "Contact", c.uuid());
}
