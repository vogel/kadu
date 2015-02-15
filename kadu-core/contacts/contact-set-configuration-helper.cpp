/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

#include "contact-set-configuration-helper.h"

ContactSet ContactSetConfigurationHelper::loadFromConfiguration(StorableObject *parent, const QString &nodeName)
{
	if (!parent->isValidStorage())
		return ContactSet();

	ConfigurationApi *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	return loadFromConfiguration(configurationStorage, contactSetNode);
}

ContactSet ContactSetConfigurationHelper::loadFromConfiguration(ConfigurationApi *configurationStorage, QDomElement contactSetNode)
{
	ContactSet result;

	QVector<QDomElement> contactElements = configurationStorage->getNodes(contactSetNode, "Contact");
	result.reserve(contactElements.count());
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

	ConfigurationApi *configurationStorage = parent->storage()->storage();
	QDomElement contactSetNode = configurationStorage->getNode(parent->storage()->point(), nodeName);

	saveToConfiguration(configurationStorage, contactSetNode, contactSet);
}

void ContactSetConfigurationHelper::saveToConfiguration(ConfigurationApi *configurationStorage, QDomElement contactSetNode, const ContactSet &contactSet)
{
	while (!contactSetNode.childNodes().isEmpty())
		contactSetNode.removeChild(contactSetNode.childNodes().at(0));

	foreach (const Contact &c, contactSet)
		configurationStorage->appendTextNode(contactSetNode, "Contact", c.uuid().toString());
}
