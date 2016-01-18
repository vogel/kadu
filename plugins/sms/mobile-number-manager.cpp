/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVector>
#include <QtCore/QtAlgorithms>

#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "storage/storage-point-factory.h"

#include "mobile-number.h"

#include "mobile-number-manager.h"

MobileNumberManager::MobileNumberManager(QObject *parent) :
		StorableObject{parent}
{
	setState(StateNotLoaded);
}

MobileNumberManager::~MobileNumberManager()
{
	qDeleteAll(Numbers);
}

void MobileNumberManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void MobileNumberManager::init()
{
	m_configurationManager->registerStorableObject(this);
}

void MobileNumberManager::done()
{
	m_configurationManager->unregisterStorableObject(this);
}

void MobileNumberManager::registerNumber(QString number, QString gatewayId)
{
	foreach (MobileNumber *n, Numbers)
		if (n->number() == number)
		{
			n->setGatewayId(gatewayId);
			return;
		}

	Numbers.append(Core::instance()->injectedFactory()->makeInjected<MobileNumber>(this, number, gatewayId));
}

void MobileNumberManager::unregisterNumber(QString number)
{
	foreach (MobileNumber *n, Numbers)
		if (n->number() == number)
		{
			Numbers.removeAll(n);
			delete n;
			break;
		}
}

std::shared_ptr<StoragePoint> MobileNumberManager::createStoragePoint()
{
	return storagePointFactory()->createStoragePoint("MobileNumbers");
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

	ConfigurationApi *configurationStorage = storage()->storage();
	QDomElement mobileNumbersNode = storage()->point();
	if (mobileNumbersNode.isNull())
		return;

	QVector<QDomElement> mobileNumberNodes = storage()->storage()->getNodes(mobileNumbersNode, "MobileNumber");
	foreach (QDomElement mobileNumberElement, mobileNumberNodes)
	{
		if (mobileNumberElement.isNull())
			continue;

		auto numberStoragePoint = std::make_shared<StoragePoint>(configurationStorage, mobileNumberElement);
		MobileNumber *number = Core::instance()->injectedFactory()->makeInjected<MobileNumber>(this);
		number->setStorage(numberStoragePoint);
		number->setState(StateNotLoaded);
		number->ensureLoaded();

		Numbers.append(number);
	}
}

void MobileNumberManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::store();

	foreach (MobileNumber *number, Numbers)
		number->ensureStored();
}

QString MobileNumberManager::gatewayId(const QString &mobileNumber)
{
	ensureLoaded();

	foreach (MobileNumber *number, Numbers)
		if (number->number() == mobileNumber)
			return number->gatewayId();

	return QString();
}

#include "moc_mobile-number-manager.cpp"
