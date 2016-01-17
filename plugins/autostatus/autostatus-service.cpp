/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>

#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "status/status-changer-manager.h"

#include "configuration/autostatus-configuration.h"
#include "autostatus-status-changer.h"

#include "autostatus-service.h"

AutostatusService::AutostatusService(QObject *parent) :
		QObject{parent}
{
	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(changeStatus()));
}

AutostatusService::~AutostatusService()
{
	Timer->stop();
}

void AutostatusService::setAutostatusConfiguration(AutostatusConfiguration *autostatusConfiguration)
{
	m_autostatusConfiguration = autostatusConfiguration;
}

void AutostatusService::setAutostatusStatusChanger(AutostatusStatusChanger *autostatusStatusChanger)
{
	m_autostatusStatusChanger = autostatusStatusChanger;
}

void AutostatusService::on()
{
	m_autostatusStatusChanger->setEnabled(true);
	Timer->start(m_autostatusConfiguration->autoTime() * 1000);
	changeStatus();
}

void AutostatusService::off()
{
	Timer->stop();
	m_autostatusStatusChanger->setEnabled(false);
}

void AutostatusService::changeStatus()
{
	if (CurrentDescription == DescriptionList.constEnd())
		CurrentDescription = DescriptionList.constBegin();

	m_autostatusStatusChanger->setConfiguration(m_autostatusConfiguration->autoStatus(), *CurrentDescription);
	CurrentDescription++;
}

bool AutostatusService::readDescriptionList()
{
	if (!QFile::exists(m_autostatusConfiguration->statusFilePath()))
	{
		MessageDialog::show(KaduIcon("dialog-information"), "Autostatus", "File does not exist !");
		return false;
	}

	DescriptionList.clear();

	QFile file(m_autostatusConfiguration->statusFilePath());

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QTextStream stream(&file);

	QString description;
	while (!stream.atEnd())
	{
		description = stream.readLine();
		if (!description.isEmpty())
			DescriptionList += description;
	}

	file.close();

	return !DescriptionList.isEmpty();
}

void AutostatusService::toggle(bool toggled)
{
	if (!toggled)
	{
		off();
//		kadu->mainMenu()->setItemChecked(menuID, false);
		DescriptionList.clear();
		return;
	}

	if (readDescriptionList())
	{
		//	kadu->mainMenu()->setItemChecked(menuID, true);
		CurrentDescription = DescriptionList.constBegin();
		on();
	}
}

#include "moc_autostatus-service.cpp"
