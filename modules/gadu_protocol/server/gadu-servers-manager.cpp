/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Ziemniak (jziemkiewicz@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QRegExp>

#include <libgadu.h>

#include "configuration/configuration-file.h"

#include "gadu-servers-manager.h"

/* Dorr: GG Server addresses update based on:
 * http://gg.thinkspire.org/ (on 25.03.2009, 08:30:01)
 * Server 91.197.13.24 is known to cause invalid password
 * errors so I have disabled it.
 */
#define GG_SERVERS_COUNT 52
const char *GaduServersManager::Ips[GG_SERVERS_COUNT] = {
	"91.214.237.2",
	"91.214.237.3",
	"91.214.237.4",
	"91.214.237.5",
	"91.214.237.6",
	"91.214.237.7",
	"91.214.237.8",
	"91.214.237.9",
	"91.214.237.10",
	"91.214.237.11",
	"91.214.237.12",
	"91.214.237.13",
	"91.214.237.14",
	"91.214.237.15",
	"91.214.237.16",
	"91.214.237.17",
	"91.214.237.18",
	"91.214.237.19",
	"91.214.237.20",
	"91.214.237.21",
	"91.214.237.22",
	"91.214.237.23",
	"91.214.237.24",
	"91.214.237.25",
	"91.214.237.26",
	"91.214.237.27",
	"91.214.237.40",
	"91.214.237.41",
	"91.214.237.44",
	"91.214.237.45",
	"91.214.237.46",
	"91.214.237.47",
	"91.214.237.48",
	"91.214.237.49",
	"91.214.237.50",
	"91.214.237.51",
	"91.214.237.52",
	"91.214.237.53",
	"91.214.237.54",
	"91.214.237.55",
	"91.214.237.56",
	"91.214.237.57",
	"91.214.237.58",
	"91.214.237.59",
	"91.214.237.62",
	"91.214.237.63",
	"91.214.237.64",
	"91.214.237.66",
	"91.214.237.67",
	"91.214.237.69",
	"91.214.237.70",
	"91.214.237.72"
};

GaduServersManager * GaduServersManager::Instance = 0;

GaduServersManager * GaduServersManager::instance()
{
	if (!Instance)
		Instance = new GaduServersManager();

	return Instance;
}

GaduServersManager::GaduServersManager()
{
	buildServerList();
}

QList<GaduServersManager::GaduServer> GaduServersManager::gaduServersFromString(const QString &serverAddress)
{
	QList<GaduServer> result;

	if (serverAddress.isEmpty() || serverAddress.startsWith(QLatin1String("0.0.0.0")))
		return result;

	QString address;
	QList<int> ports;
	QRegExp addressPortRegexp( "^(.+):(\\d+)$" ); // X:Y
	if (serverAddress.contains(addressPortRegexp))
	{
		address = addressPortRegexp.cap(1);
		int port = addressPortRegexp.cap(2).toInt();
		ports << port;
	}
	else
	{
		address = serverAddress;
		ports = AllPorts;
	}

	QList<QString> servers;
	QRegExp ipRangeRegexp("^(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)-(\\d+)$"); // X.X.X.X-X
	if (address.contains(ipRangeRegexp))
	{
		int a = ipRangeRegexp.cap(1).toInt();
		int b = ipRangeRegexp.cap(2).toInt();
		int c = ipRangeRegexp.cap(3).toInt();
		int d1 = ipRangeRegexp.cap(4).toInt();
		int d2 = ipRangeRegexp.cap(5).toInt();
		for (int d = d1; d <= d2; ++d)
			servers << QString("%1.%2.%3.%4").arg(a).arg(b).arg(c).arg(d);
	}
	else
		servers << address;

	QHostAddress ip;
	foreach (QString server, servers)
		if (ip.setAddress(server))
			foreach (int port, ports)
				result.append(GaduServer(ip, port));

	return result;
}

void GaduServersManager::buildServerList()
{
	GoodServers.clear();
	BadServers.clear();
	AllServers.clear();
	AllPorts.clear();

	int LastGoodPort = config_file.readNumEntry("Network", "LastServerPort",
			config_file.readNumEntry("Network", "DefaultPort", 8074));

	if (8074 == LastGoodPort || 443 == LastGoodPort)
		AllPorts << LastGoodPort;
	if (8074 != LastGoodPort)
		AllPorts << 8074;
	if (443 != LastGoodPort)
		AllPorts << 443;

	if (config_file.readBoolEntry("Network", "isDefServers", true))
	{
		GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
		GoodServers << gaduServersFromString(config_file.readEntry("Network", "LastServerIP"));
		for (int i = 0; i < GG_SERVERS_COUNT; ++i)
			GoodServers << gaduServersFromString(QString::fromLatin1(Ips[i]));
	}
	else
	{
		QStringList servers = config_file.readEntry("Network", "Server").split(';', QString::SkipEmptyParts);
		foreach (const QString &server, servers)
			GoodServers << gaduServersFromString(server.trimmed());
		GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
		GoodServers << gaduServersFromString(config_file.readEntry("Network", "LastServerIP"));
	}

	AllServers = GoodServers;
}

void GaduServersManager::configurationUpdated()
{
	buildServerList();
}

GaduServersManager::GaduServer GaduServersManager::getServer()
{
	if (0 == GoodServers.count())
	{
		GoodServers = BadServers;
		BadServers.clear();
	}

	if (0 == GoodServers.count())
		return GaduServer(QHostAddress(), 0);

	return GoodServers[0];
}

const QList<GaduServersManager::GaduServer> & GaduServersManager::getServersList()
{
	return AllServers;
}

void GaduServersManager::markServerAsGood(GaduServersManager::GaduServer server)
{
	config_file.writeEntry("Network", "LastServerIP", server.first.toString());
	config_file.writeEntry("Network", "LastServerPort", server.second);
}

void GaduServersManager::markServerAsBad(GaduServersManager::GaduServer server)
{
	GoodServers.removeAll(server);
	BadServers.append(server);
}
