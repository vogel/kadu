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

#include "sms-gateway-manager.h"

SmsGatewayManager * SmsGatewayManager::Instance = 0;

SmsGatewayManager * SmsGatewayManager::instance()
{
	if (0 == Instance)
	{
		Instance = new SmsGatewayManager();
	}
	return Instance;
}

void SmsGatewayManager::registerGateway(SmsGateway *gateway)
{
	QStringList priority = config_file.readEntry("SMS", "Priority").split(";");
	if (!priority.contains(gateway->name()))
	{
		priority += gateway->name();
		config_file.writeEntry("SMS", "Priority", priority.join(";"));
	}
	Gateways.insert(gateway->name(), gateway);
}

void SmsGatewayManager::unregisterGateway(QString name)
{
	Gateways.remove(name);
}

SmsGateway * SmsGatewayManager::byId(QString gatewayId)
{
	return Gateways.contains(gatewayId) ? Gateways.value(gatewayId) : 0;
}
