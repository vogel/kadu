/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QSysInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/updates-dialog.h"
#include "debug.h"

#include "updates.h"

Updates::Updates(QObject *parent) :
		QObject(parent), UpdateChecked{false}
{
	kdebugf();
	buildQuery();
	triggerAllAccountsRegistered();
}

Updates::~Updates()
{
}

void Updates::accountRegistered(Account account)
{
	connect(account, SIGNAL(connected()), this, SLOT(run()));
}

void Updates::accountUnregistered(Account account)
{
	disconnect(account, 0, this, 0);
}

void Updates::buildQuery()
{
	Query = QString("/update-new.php?uuid=%1&version=%2").arg(ConfigurationManager::instance()->uuid().toString()).arg(Core::version());

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "SendSysInfo"), true)
	{
		QString platform("&system=");
#if defined(Q_OS_LINUX)
                platform.append("Linux-");

                QFile issue("/etc/issue");
                if (issue.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                        QString tmp = issue.readLine();
                        tmp.truncate(tmp.indexOf(" "));
                        platform.append(tmp);
                        issue.close();
                }
                else
                        platform.append("Unknown");
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
			case QSysInfo::MV_SNOWLEOPARD:
				platform.append("MacOSX-SnowLeopard");
				break;
			default:
				platform.append("MacOSX-Unknown");
				break;
		}
#elif defined(Q_OS_WIN)
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
			case QSysInfo::WV_WINDOWS7:
				platform.append("Windows7");
				break;
	    		default:
				platform.append("Windows-Unknown");
				break;
		}
#elif defined(Q_OS_HAIKU)
                platform.append("Haiku OS");
#else
		platform.append("Unknown");
#endif
		Query.append(platform);
	}
}

void Updates::run()
{
	kdebugf();

	if (UpdateChecked)
		return;

	UpdateChecked = true;

	auto manager = new QNetworkAccessManager{this};
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(gotUpdatesInfo(QNetworkReply*)));

	manager->get(QNetworkRequest{QUrl{QLatin1String{"http://www.kadu.im"} + Query}});
}

bool Updates::isNewerVersionThan(const QString &version)
{
	QStringList thisVersion = stripVersion(Core::version()).split('.');
	QStringList queryVersion = stripVersion(version).split('.');

	for (int i = 0, end = qMin(thisVersion.size(), queryVersion.size()); i < end; ++i)
		if (queryVersion.at(i).toInt() != thisVersion.at(i).toInt())
			return queryVersion.at(i).toInt() > thisVersion.at(i).toInt();

	return (queryVersion.size() > thisVersion.size());
}

QString Updates::stripVersion(const QString &version)
{
	Qt::CaseSensitivity cs = Qt::CaseInsensitive;

	// We don't want to compare git versions at all.
	if (version.contains("-g", cs))
		return "9999";

	QString strippedVersion = version;
	// Use negative numbers so that 0.1.0 is considered newer than 0.1-alpha10.
	if (strippedVersion.contains("-alpha", cs))
		strippedVersion.replace("-alpha", ".-3.", cs);
	else if (strippedVersion.contains("-beta", cs))
		strippedVersion.replace("-beta", ".-2.", cs);
	else if (strippedVersion.contains("-rc", cs))
		strippedVersion.replace("-rc", ".-1.", cs);
	else
		strippedVersion.append(".0");

	return strippedVersion;
}

void Updates::gotUpdatesInfo(QNetworkReply *reply)
{
	kdebugf();

	reply->deleteLater();
	deleteLater();

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "CheckUpdates"))
	{
		auto newestVersion = QString::fromUtf8(reply->readAll());
		if (newestVersion.size() > 31)
		{
			kdebugmf(KDEBUG_WARNING, "cannot obtain update info\n");
			return;
		}

		if (isNewerVersionThan(newestVersion))
		{
			UpdatesDialog *dialog = new UpdatesDialog(newestVersion, Core::instance()->kaduWindow());
			dialog->show();
		}
	}
}

#include "moc_updates.cpp"
