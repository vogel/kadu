/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "mobile-number.h"

#include "mobile-number-manager.h"

MobileNumberManager * MobileNumberManager::Instance = 0;

MobileNumberManager * MobileNumberManager::instance()
{
	if (0 == Instance)
		Instance = new MobileNumberManager();

	return Instance;
}

void MobileNumberManager::registerNumber(QString number, QString gatewayId)
{
	Numbers.insert(new MobileNumber(number, gatewayId), gatewayId);
}

void MobileNumberManager::unregisterNumber(QString number)
{
	foreach (MobileNumber *n, Numbers.keys())
		if (n->number() == number)
			Numbers.remove(n);
}

StoragePoint * MobileNumberManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("MobileNumbers"));
}

StorableObject * MobileNumberManager::storageParent()
{
	return 0;
}

void MobileNumberManager::load()
{
  	if (!isValidStorage())
		return;

	StorableObject::load();

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement mobileNumbersNode = storage()->point();
	if (mobileNumbersNode.isNull())
		return;

	QList<QDomElement> mobileNumberNodes = storage()->storage()->getNodes(mobileNumbersNode, "MobileNumber");
	foreach (QDomElement mobileNumberElement, mobileNumberNodes)
	{
		if (mobileNumberElement.isNull())
			continue;

		StoragePoint *numberStoragePoint = new StoragePoint(configurationStorage, mobileNumberElement);
		MobileNumber *number = new MobileNumber();
		number->setStorage(numberStoragePoint);
		number->ensureLoaded();

		Numbers.insert(number, number->gatewayId());
	}
}

void MobileNumberManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::store();

	foreach (MobileNumber *number, Numbers.keys())
		number->store();
}
