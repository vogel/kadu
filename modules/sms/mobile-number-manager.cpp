/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"

#include "mobile-number-manager.h"
#include "sms-gateway-manager.h"

MobileNumberManager * MobileNumberManager::Instance = 0;

MobileNumberManager * MobileNumberManager::instance()
{
	if (0 == Instance)
	{
		Instance = new MobileNumberManager();
	}
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

SmsGateway * MobileNumberManager::gateway(QString number)
{
	foreach (MobileNumber *n, Numbers.keys())
		if (n->number() == number)
			return SmsGatewayManager::instance()->byId(Numbers.value(n));
	return 0;
}

StoragePoint * MobileNumberManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("MobileNumbers"));
}

void MobileNumberManager::load()
{
  	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement mobileNumbersNode = storage()->point();
	if (mobileNumbersNode.isNull())
		return;

	QDomNodeList mobileNumberNodes = storage()->storage()->getNodes(mobileNumbersNode, "MobileNumber");
	int count = mobileNumberNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement mobileNumberElement = mobileNumberNodes.item(i).toElement();
		if (mobileNumberElement.isNull())
			continue;

		StoragePoint *numberStoragePoint = new StoragePoint(configurationStorage, mobileNumberElement);
		MobileNumber *number = new MobileNumber();
		number->setStorage(numberStoragePoint);
		number->load();
		Numbers.insert(number, number->gatewayId());
	}
}

void MobileNumberManager::store()
{
	if (!isValidStorage())
		return;

	QDomElement mobileNumbersNode = storage()->point();

	foreach (MobileNumber *number, Numbers.keys())
		number->store();
}

MobileNumber::MobileNumber(QString number, QString gatewayId) : Number(number), GatewayId(gatewayId)
{
}

StoragePoint * MobileNumber::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("MobileNumber"));
}

void MobileNumber::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	Number = loadValue<QString>("Number");
	GatewayId = loadValue<QString>("Gateway");
}

void MobileNumber::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Number", Number, true);
	storeValue("Gateway", GatewayId,true);
}