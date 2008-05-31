/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QHttp>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "kadu.h"
#include "message_box.h"

#include "updates.h"

Updates *Updates::instance = 0;
bool Updates::UpdateChecked = false;
QDateTime Updates::LastUpdateCheck;

Updates::Updates(UinType uin)
	: query(QString("/update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION)))
{
	kdebugf();

	httpClient = new QHttp("www.kadu.net");

	kdebugf2();
}

Updates::~Updates()
{
	kdebugf();
	delete httpClient;
	httpClient = 0;
}

void Updates::run()
{
	kdebugf();

	httpClient->get(query);

	kdebugf2();
}

bool Updates::ifNewerVersion(const QString &newestversion)
{
	return (newestversion != QString(VERSION));
}

void Updates::initModule()
{
	kdebugf();

	QDateTime actualtime = QDateTime::currentDateTime();
	LastUpdateCheck.setTime_t(config_file.readNumEntry("General", "LastUpdateCheck"));

	if (!UpdateChecked && LastUpdateCheck.secsTo(actualtime) >= 3600)
	{
		UinType myUin = (UinType)kadu->myself().ID("Gadu").toUInt();
		if (myUin)
		{
			instance = new Updates(myUin);
			connect(instance->httpClient, SIGNAL(readyRead(const QHttpResponseHeader &)),
					instance, SLOT(gotUpdatesInfo(const QHttpResponseHeader &)));
			connect(gadu, SIGNAL(connected()), instance, SLOT(run()));
		}
	}

	kdebugf2();
}

void Updates::closeModule()
{
	kdebugf();

	if (instance)
	{
		instance->deleteLater();
		instance = 0;
	}

	kdebugf2();
}

void Updates::gotUpdatesInfo(const QHttpResponseHeader &responseHeader)
{
	kdebugf();

	QByteArray data = httpClient->readAll();

	if (config_file.readBoolEntry("General", "CheckUpdates"))
	{
		unsigned int size = data.size();
		if (size > 31)
		{
			kdebugmf(KDEBUG_WARNING, "cannot obtain update info\n");
			disconnect(gadu, SIGNAL(connected()), this, SLOT(run()));
			deleteLater();
			kdebugf2();
			return;
		}
		
		QString newestversion(data);
		if (ifNewerVersion(newestversion))
			MessageBox::msg(tr("The newest Kadu version is %1").arg(newestversion), false, "Information", kadu);
	}
	disconnect(gadu, SIGNAL(connected()), this, SLOT(run()));
	UpdateChecked = true;
	config_file.writeEntry("General", "LastUpdateCheck", QDateTime(QDate(1970, 1, 1)).secsTo(QDateTime::currentDateTime()));
	closeModule();

	kdebugf2();
}
