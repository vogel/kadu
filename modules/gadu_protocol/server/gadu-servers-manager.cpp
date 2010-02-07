/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <libgadu.h>

#include "configuration/configuration-file.h"

#include "gadu-servers-manager.h"

/* Dorr: GG Server addresses update based on:
 * http://gg.thinkspire.org/ (on 25.03.2009, 08:30:01)
 * Server 91.197.13.24 is known to cause invalid password
 * errors so I have disabled it.
 */
#define GG_SERVERS_COUNT 45
const char *GaduServersManager::Ips[GG_SERVERS_COUNT] = {
	"91.197.13.2",
	"91.197.12.4",
	"91.197.12.5",
	"91.197.12.6",
	"91.197.13.7",
	"91.197.13.8",
	"91.197.13.11",
	"91.197.13.12",
	"91.197.13.13",
	"91.197.13.14",
	"91.197.13.17",
	"91.197.13.18",
	"91.197.13.19",
	"91.197.13.20",
	"91.197.13.21",
//	"91.197.13.24",
	"91.197.13.25",
	"91.197.13.26",
	"91.197.13.27",
	"91.197.13.28",
	"91.197.13.29",
	"91.197.13.31",
	"91.197.13.32",
	"91.197.13.33",
	"91.197.13.49",
	"91.197.13.51",
	"91.197.13.66",
	"91.197.13.67",
	"91.197.13.68",
	"91.197.13.69",
	"91.197.13.70",
	"91.197.13.71",
	"91.197.13.72",
	"91.197.13.73",
	"91.197.13.74",
	"91.197.13.75",
	"91.197.13.76",
	"91.197.13.77",
	"91.197.13.78",
	"91.197.13.79",
	"91.197.13.80",
	"91.197.13.81",
	"91.197.13.82",
	"91.197.13.83",
	"91.197.13.84",
	"91.197.13.93"
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

void GaduServersManager::buildServerList()
{
	GoodServers.clear();
	BadServers.clear();
	AllServers.clear();

	int LastGoodPort = config_file.readNumEntry("Network", "LastServerPort",
			config_file.readNumEntry("Network", "DefaultPort", 8074));

	if (8074 == LastGoodPort || 443 == LastGoodPort)
		AllPorts << LastGoodPort;
	if (8074 != LastGoodPort)
		AllPorts << 8074;
	if (443 != LastGoodPort)
		AllPorts << 443;

	QHostAddress ip;

	// for GG hub
	ip.setAddress((quint32)0);
	GoodServers.append(qMakePair(ip, 0));

	if (ip.setAddress(config_file.readEntry("Network", "LastServerIP")))
		foreach (int port, AllPorts)
			GoodServers.append(qMakePair(ip, port));

	if (config_file.readBoolEntry("Network", "isDefServers", true))
	{
		for (int i = 0; i < GG_SERVERS_COUNT; i++)
			if (ip.setAddress(QString::fromLatin1(Ips[i])))
			{
				foreach (int port, AllPorts)
					GoodServers.append(qMakePair(ip, port));
					AllServers.push_back(ip);
			}
	}
	else
	{
		QStringList servers = config_file.readEntry("Network", "Server").split(";", QString::SkipEmptyParts);

		foreach (const QString &server, servers)
			if (ip.setAddress(server))
			{
				foreach (int port, AllPorts)
					GoodServers.append(qMakePair(ip, port));
					AllServers.push_back(ip);
			}
	}
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
		return qMakePair(QHostAddress(), 0);

	return GoodServers[0];
}

const QList<QHostAddress>& GaduServersManager::getServersList()
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
