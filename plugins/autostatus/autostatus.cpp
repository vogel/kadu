/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "autostatus-status-changer.h"

#include "autostatus.h"

Autostatus * Autostatus::Instance = 0;

void Autostatus::createInstance()
{
	if (!Instance)
		Instance = new Autostatus();
}

void Autostatus::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Autostatus::Autostatus()
{
	MyStatusChanger = new AutostatusStatusChanger(this);
	StatusChangerManager::instance()->registerStatusChanger(MyStatusChanger);

	Timer = new QTimer(this);
	connect(Timer, SIGNAL(timeout()), this, SLOT(changeStatus()));
}


Autostatus::~Autostatus()
{
	StatusChangerManager::instance()->unregisterStatusChanger(MyStatusChanger);

	Timer->stop();
}

void Autostatus::on()
{
	MyStatusChanger->setEnabled(true);
	Timer->start(Configuration.autoTime() * 1000);
	changeStatus();
}

void Autostatus::off()
{
	Timer->stop();
	MyStatusChanger->setEnabled(false);
}

void Autostatus::changeStatus()
{
	if (CurrentDescription == DescriptionList.constEnd())
		CurrentDescription = DescriptionList.constBegin();

	MyStatusChanger->setConfiguration(Configuration.autoStatus(), *CurrentDescription);
	CurrentDescription++;
}

bool Autostatus::readDescriptionList()
{
	if (!QFile::exists(Configuration.statusFilePath()))
	{
		MessageDialog::show(KaduIcon("dialog-information"), "Autostatus", "File does not exist !");
		return false;
	}

	DescriptionList.clear();

	QFile file(Configuration.statusFilePath());

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

void Autostatus::toggle(bool toggled)
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
