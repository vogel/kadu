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

MobileNumberManager * MobileNumberManager::Instance = 0;

SMSAPI MobileNumberManager * MobileNumberManager::instance()
{
	if (0 == Instance)
	{
		Instance = new MobileNumberManager();
	}
	return Instance;
}

void MobileNumberManager::registerNumber(QString &number, QString &gatewayId)
{
// 	QStringList priority = config_file.readEntry("SMS", "Priority").split(";");
// 	if (!priority.contains(number->name()))
// 	{
// 		priority += number->name();
// 		config_file.writeEntry("SMS", "Priority", priority.join(";"));
// 	}
// 	Gateways.insert(number->name(), number);
}

void MobileNumberManager::unregisterNumber(QString &number)
{
//	Gateways.remove(name);
}


void MobileNumberManager::load()
{
  
}

void MobileNumberManager::store()
{
  
}