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
	QStringList servers = config_file.readEntry("Network", "Server").split(";", QString::SkipEmptyParts);

	foreach (const QString &server, servers)
		if (ip.setAddress(server))
			Servers.append(ip);

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
