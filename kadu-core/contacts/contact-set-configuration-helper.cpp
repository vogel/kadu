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

	QList<QDomElement> contactElements = configurationStorage->getNodes(contactSetNode, "Contact");
	foreach (QDomElement contactElement, contactElements)
	{
		Contact contact = ContactManager::instance()->byUuid(contactElement.text());
		// TODO: 0.6.6 this only for compatibility with previous builds of 0.6.6
		if (contact.isNull())
			contact = BuddyManager::instance()->byUuid(contactElement.text()).prefferedContact();
		if (!contact.isNull())
			result.insert(contact);
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

	foreach (const Contact &c, contactSet)
		configurationStorage->appendTextNode(contactSetNode, "Contact", c.uuid());
}
