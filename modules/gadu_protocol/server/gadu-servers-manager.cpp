/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <libgadu.h>

#include "config_file.h"

#include "gadu-servers-manager.h"

#define GG_SERVERS_COUNT 39
const char *GaduServersManager::Ips[GG_SERVERS_COUNT] = {
	"91.197.13.24",
	"91.197.13.33",
	"91.197.13.2",
	"91.197.13.4",
	"91.197.13.5",
	"91.197.13.6",
	"91.197.13.7",
	"91.197.13.14",
	"91.197.13.11",
	"91.197.13.12",
	"91.197.13.13",
	"91.197.13.17",
	"91.197.13.18",
	"91.197.13.19",
	"91.197.13.20",
	"91.197.13.21",
	"91.197.13.25",
	"91.197.13.26",
	"91.197.13.27",
	"91.197.13.28",
	"91.197.13.29",
	"91.197.13.31",
	"91.197.13.32",
	"217.17.41.83",
	"217.17.41.84",
	"217.17.41.85",
	"217.17.41.86",
	"217.17.41.87",
	"217.17.41.88",
	"217.17.41.89",
	"217.17.41.90",
	"217.17.41.91",
	"217.17.41.92",
	"217.17.41.93",
	"217.17.45.143",
	"217.17.45.144",
	"217.17.45.145",
	"217.17.45.146",
	"217.17.45.133"
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
	int LastGoodPort = config_file.readNumEntry("Network", "LastServerPort",
			config_file.readNumEntry("Network", "DefaultPort", 8074));

	if (8074 == LastGoodPort || 443 == LastGoodPort)
		AllPorts << LastGoodPort;
	if (8074 != LastGoodPort)
		AllPorts << 8074;
	if (443 != LastGoodPort)
		AllPorts << 443;

	QHostAddress ip;
	if (ip.setAddress(config_file.readEntry("Network", "LastServerIP")))
		foreach (int port, AllPorts)
			GoodServers.append(qMakePair(ip, port));

	if (config_file.readBoolEntry("Network", "isDefServers", true))
	{
		for (int i = 0; i < GG_SERVERS_COUNT; i++)
			if (ip.setAddress(QString::fromLatin1(Ips[i])))
				foreach (int port, AllPorts)
					GoodServers.append(qMakePair(ip, port));
	}
	else
	{
		QStringList servers = config_file.readEntry("Network", "Server").split(";", QString::SkipEmptyParts);

		foreach (const QString &server, servers)
			if (ip.setAddress(server))
				foreach (int port, AllPorts)
					GoodServers.append(qMakePair(ip, port));
	}
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
