/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtXml/QDomElement>

#include "configuration/xml-configuration-file.h"
#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/buddy-preferred-manager.h"
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
	foreach (const QDomElement &contactElement, contactElements)
	{
		Contact contact = ContactManager::instance()->byUuid(contactElement.text());
		if (!contact.isNull())
			result.insert(contact);
	}

	return result;
}

void ContactSetConfigurationHelper::saveToConfiguration(StorableObject *parent, const QString &nodeName, const ContactSet &contactSet)
{
	if (!parent->isValidStorage())
		return;

	XmlConfigFile *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	saveToConfiguration(configurationStorage, contactSetNode, contactSet);
}

void ContactSetConfigurationHelper::saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, const ContactSet &contactSet)
{
	while (contactSetNode.childNodes().count())
		contactSetNode.removeChild(contactSetNode.childNodes().at(0));

	foreach (const Contact &c, contactSet)
		configurationStorage->appendTextNode(contactSetNode, "Contact", c.uuid());
}
