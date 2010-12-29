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

#include <QtCore/QFile>
#include <QtCore/QRegExp>

#include <libgadu.h>

#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"

#include "gadu-servers-manager.h"

GaduServersManager * GaduServersManager::Instance = 0;

void GaduServersManager::createInstance()
{
	if (!Instance)
		Instance = new GaduServersManager();
}

void GaduServersManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
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
	foreach (const QString &server, servers)
		if (ip.setAddress(server))
			foreach (int port, ports)
				result.append(GaduServer(ip, port));

	return result;
}

void GaduServersManager::loadServerListFromFile(const QString &fileName)
{
	GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
	GoodServers << gaduServersFromString(config_file.readEntry("Network", "LastServerIP"));

	QFile file(fileName);

	if (!file.open(QFile::ReadOnly))
		return;

	QTextStream serversStream(&file);

	while (!serversStream.atEnd())
	{
		QString server = serversStream.readLine();
		GoodServers << gaduServersFromString(server);
	}

	file.close();
}

void GaduServersManager::loadServerListFromString(const QString& data)
{
	QStringList servers = data.split(';', QString::SkipEmptyParts);

	foreach (const QString &server, servers)
		GoodServers << gaduServersFromString(server.trimmed());
	GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
	GoodServers << gaduServersFromString(config_file.readEntry("Network", "LastServerIP"));
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
		loadServerListFromFile(dataPath("kadu/modules/data/gadu_protocol/servers.txt"));
	else
		loadServerListFromString(config_file.readEntry("Network", "Server"));

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
