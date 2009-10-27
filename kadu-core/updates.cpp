/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtNetwork/QHttp>
#include <QtCore/QSysInfo>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"
#include "debug.h"
#include "kadu-config.h"
#include "../modules/gadu_protocol/gadu-protocol.h"

#include "updates.h"

Updates *Updates::instance = 0;
bool Updates::UpdateChecked = false;
QDateTime Updates::LastUpdateCheck;

Updates::Updates(UinType uin)
	: query(QString("/update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION)))
{
	kdebugf();

	httpClient = new QHttp("www.kadu.net");
	
	if (config_file.readBoolEntry("General", "SendSysInfo"), true)
	{
		QString platform("&system=");
#if defined(Q_OS_LINUX)
		/* TODO: obtain the distribution name and version */
		platform.append("Linux");
#elif defined(Q_OS_FREEBSD)
		platform.append("FreeBSD");
#elif defined(Q_OS_NETBSD)
		platform.append("NetBSD");
#elif defined(Q_OS_OPENBSD)
		platform.append("OpenBSD");
#elif defined(Q_OS_SOLARIS)
		platform.append("Solaris");
#elif defined(Q_OS_MAC)
		switch (QSysInfo::MacintoshVersion)
		{
			case QSysInfo::MV_PANTHER:
				platform.append("MacOSX-Panther");
				break;
			case QSysInfo::MV_TIGER:
				platform.append("MacOSX-Tiger");
				break;
			case QSysInfo::MV_LEOPARD:
				platform.append("MacOSX-Leopard");
				break;
			default:
				platform.append("MacOSX-Unknown");
				break;
		}
#elif defined(Q_OS_WIN32)
		switch (QSysInfo::WindowsVersion)
		{
			case QSysInfo::WV_95:
				platform.append("Windows95");
				break;
			case QSysInfo::WV_98:
				platform.append("Windows98");
				break;
			case QSysInfo::WV_Me:
				platform.append("WindowsME");
				break;
			case QSysInfo::WV_NT:
				platform.append("WindowsNT");
				break;
			case QSysInfo::WV_2000:
				platform.append("Windows2000");
				break;
			case QSysInfo::WV_XP:
				platform.append("WindowsXP");
				break;
			case QSysInfo::WV_2003:
				platform.append("Windows2003");
				break;
			case QSysInfo::WV_VISTA:
				platform.append("WindowsVista");
				break;
	    		default:
				platform.append("Windows-Unknown");
				break;
		}
#else
		platform.append("Unknown");
#endif
		query.append(platform);
	}

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
	Qt::CaseSensitivity cs = Qt::CaseInsensitive; // find and replace are NOT case sensitive
 	QString version = stripversion;
 
	if (version.contains("-svn", cs))
		version.replace("-svn", "01", cs);
	else if (version.contains("-alpha", cs))
		version.replace("-alpha", "02", cs);
	else if (version.contains("-beta", cs))
		version.replace("-beta", "03", cs);
	else if (version.contains("-rc", cs))
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
	{/* TODO: 0.6.6
		UinType myUin = (UinType)kadu->myself().ID("Gadu").toUInt();
		if (myUin)
		{
			instance = new Updates(myUin);
			connect(instance->httpClient, SIGNAL(readyRead(const QHttpResponseHeader &)),
					instance, SLOT(gotUpdatesInfo(const QHttpResponseHeader &)));

			GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
			connect(gadu, SIGNAL(connected()), instance, SLOT(run()));
		}*/
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

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocolHandler());
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
			MessageBox::msg(tr("The newest Kadu version is %1").arg(newestversion), false, "Information", Core::instance()->kaduWindow());
	}
	disconnect(gadu, SIGNAL(connected()), this, SLOT(run()));
	UpdateChecked = true;
	config_file.writeEntry("General", "LastUpdateCheck", QDateTime(QDate(1970, 1, 1)).secsTo(QDateTime::currentDateTime()));
	closeModule();

	kdebugf2();
}
