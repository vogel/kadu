/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qnetwork.h>
#include <qurloperator.h>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "message_box.h"
#include "updates.h"

Updates *Updates::instance = NULL;
bool Updates::UpdateChecked = false;
QDateTime Updates::LastUpdateCheck;

Updates::Updates(UinType uin) : query(QString("update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION))), op(0)
{
	kdebugf();
	qInitNetworkProtocols();
	op = new QUrlOperator("http://www.kadu.net");
	kdebugf2();
}

Updates::~Updates()
{
	kdebugf();
	delete op;
}

void Updates::run()
{
	kdebugf();
	op->get(query);
	kdebugf2();
}

bool Updates::ifNewerVersion(const QString &newestversion)
{
	QString actual = stripVersion(VERSION);
	QString newest = stripVersion(newestversion);

	if (newest.length() > actual.length())
		actual.append(QString().fill('0', newest.length() - actual.length()));
	else
		newest.append(QString().fill('0', actual.length() - newest.length()));

	return (newest.toUInt() > actual.toUInt());
}

QString Updates::stripVersion(const QString stripversion)
{
	bool cs = false; // find and replace are NOT case sensitive
	QString version = stripversion;

	if (version.find("-svn", 0, cs) != -1)
		version.replace("-svn", "01", cs);
	else if (version.find("-alpha", 0, cs) != -1)
		version.replace("-alpha", "02", cs);
	else if (version.find("-beta", 0, cs) != -1)
		version.replace("-beta", "03", cs);
	else if (version.find("-rc", 0, cs) != -1)
		version.replace("-rc", "04", cs);
	else
		version.append("05");

	return (version.remove("."));
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
			connect(instance->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
					instance, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
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

void Updates::gotUpdatesInfo(const QByteArray &data, QNetworkOperation * /*op*/)
{
	kdebugf();
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
