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
	"91.197.13.2",
	"91.197.12.4",
	"91.197.12.5",
	"91.197.12.6",
	"91.197.13.11",
	"91.197.13.12",
	"91.197.13.13",
	"91.197.13.14",
	"91.197.13.17",
	"91.197.13.18",
	"91.197.13.19",
	"91.197.13.20",
	"91.197.13.21",
	"91.197.13.24",
	"91.197.13.25",
	"91.197.13.26",
	"91.197.13.27",
	"91.197.13.28",
	"91.197.13.29",
	"91.197.13.31",
	"91.197.13.32",
	"91.197.13.33",
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
	"217.17.45.133",
	"217.17.45.143",
	"217.17.45.144",
	"217.17.45.145",
	"217.17.45.146",
	"217.17.45.147"
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
	Ports << 8074;
	Ports << 443;

	LastGoodPort = config_file.readNumEntry("Network", "LastServerPort",
			config_file.readNumEntry("Network", "DefaultPort", 8074));
	if (!Ports.contains(LastGoodPort))
		Ports << LastGoodPort;

	LastPortTriedIndex = Ports.indexOf(LastGoodPort);

	QHostAddress ip;
	if (config_file.readBoolEntry("Network", "isDefServers", true))
	{
		for (int i = 0; i < GG_SERVERS_COUNT; i++)
			if (ip.setAddress(QString::fromLatin1(Ips[i])))
				Servers.append(ip);
	}
	else
	{
		QStringList servers = config_file.readEntry("Network", "Server").split(";", QString::SkipEmptyParts);

		foreach (const QString &server, servers)
			if (ip.setAddress(server))
				Servers.append(ip);
	}

	if (LastGoodServer.setAddress(config_file.readEntry("Network", "LastServerIP")))
		if (!Servers.contains(LastGoodServer))
			Servers << LastGoodServer;

	LastServerTriedIndex = LastGoodServer.isNull()
		? 0
		: Servers.indexOf(LastGoodServer);
}

QHostAddress GaduServersManager::getGoodServer()
{
	QHostAddress result;

	if (!LastGoodServer.isNull())
	{
		result = LastGoodServer;
		LastGoodServer = QHostAddress();
		return result;
	}

	if (Servers.empty())
		return QHostAddress();

	result = Servers[LastServerTriedIndex];
	LastServerTriedIndex++;
	if (LastServerTriedIndex >= Servers.count())
	{
		LastServerTriedIndex = Servers.count() - 1;
		LastPortTriedIndex++;
		if (LastPortTriedIndex >= Ports.count())
			LastPortTriedIndex = 0;
	}

	return result;
}

void GaduServersManager::markServerAsGood(const QHostAddress &goodServer)
{
	LastGoodServer = goodServer;
	if (!Servers.contains(LastGoodServer))
		Servers << LastGoodServer;
	LastServerTriedIndex = Servers.indexOf(LastGoodServer);

	config_file.writeEntry("Network", "LastServerIP", LastGoodServer.toString());
}

void GaduServersManager::markPortAsGood(int port)
{
	LastGoodPort = port;
	if (!Ports.contains(LastGoodPort))
		Ports << LastGoodPort;
	LastPortTriedIndex = Ports.indexOf(LastGoodPort);

	config_file.writeEntry("Network", "LastServerPort", LastGoodPort);
}
