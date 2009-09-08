/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtGui/QMenu>
#include <QtGui/QSpinBox>
#include <QtGui/QMessageBox>

#include "kadu.h"
#include "config_file.h"
#include "debug.h"
#include "misc.h"
#include "gadu.h"

#include "autostatus.h"
#include "power_status_changer.h"

Autostatus *autostatus;

extern "C" KADU_EXPORT int autostatus_init()
{
	autostatus = new Autostatus();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/autostatus.ui"), autostatus);

	return 0;
}

extern "C" KADU_EXPORT void autostatus_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/autostatus.ui"), autostatus);
	delete autostatus;
	autostatus = 0;
}

Autostatus::Autostatus()
{
	addDefaultConfiguration();
	powerStatusChanger = new PowerStatusChanger();

	autostatusActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "autostatusAction",
		this, SLOT(onAutostatus(QAction *, bool)),
		"Autostatus", tr("&Autostatus"), true
	);
	kadu->insertMenuActionDescription(0, autostatusActionDescription);

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(changeStatus()));
	enabled = false;
}


Autostatus::~Autostatus()
{
	off();
	disconnect(timer, SIGNAL(timeout()), this, SLOT(changeStatus()));
	delete timer;
	delete powerStatusChanger;
	
	kadu->removeMenuActionDescription(autostatusActionDescription);
	delete autostatusActionDescription;
}

void Autostatus::on()
{
	autoTime = config_file.readNumEntry("PowerKadu", "autostatus_time", 10);
	timer->start(autoTime * 1000);
}

void Autostatus::off()
{
	timer->stop();
}

void Autostatus::changeStatus()
{
	kdebugf();

	if (it == statusList.end())
		it = statusList.begin();

	autoStatus = config_file.readNumEntry("PowerKadu", "autoStatus");
	switch(autoStatus)
	{
		case 0:
			powerStatusChanger->setOnline(*it);
			break;
		case 1:
			powerStatusChanger->setBusy(*it);
			break;
		case 2:
			powerStatusChanger->setInvisible(*it);
			break;
		case 3:
			powerStatusChanger->setTalkWithMe(*it);
			break;
		case 4:
			powerStatusChanger->setDoNotDisturb(*it);
			break;
	}
	it++;
	kdebugf2();
}

void Autostatus::onAutostatus(QAction *sender, bool toggled)
{
	if (toggled)
	{
		currStat = gadu->currentStatus().index();
		currDesc = gadu->currentStatus().description();

		if (QFile::exists(config_file.readEntry("PowerKadu", "status_file_path")))
		{
			QFile file(config_file.readEntry("PowerKadu", "status_file_path"));
		
			QString desc;
			
			if (file.open(IO_ReadOnly))
			{
				QTextStream stream(&file);

				while (!stream.atEnd())
				{
					desc = stream.readLine();
			
					if((desc != "") && (strlen(desc.latin1()) <= 70))
						statusList += desc;
				}
			}

			file.close();

//			kadu->mainMenu()->setItemChecked(menuID, true);
			on();

			it = statusList.begin();
		}
		else
		{
			QMessageBox::information(NULL, "Autostatus", "File does not exist ! ");
		}
	}
	else
	{
		off();
//		kadu->mainMenu()->setItemChecked(menuID, false);
		powerStatusChanger->setIndex(currStat, currDesc);
		statusList.clear();
	}
}

void Autostatus::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

void Autostatus::configurationUpdated()
{
}

void Autostatus::addDefaultConfiguration()
{
	config_file.addVariable("PowerKadu", "status_file_path", ggPath("autostatus.list"));
}
