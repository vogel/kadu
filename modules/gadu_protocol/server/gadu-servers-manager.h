/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SERVERS_MANAGER_H
#define GADU_SERVERS_MANAGER_H

#include <QtCore/QList>
#include <QtNetwork/QHostAddress>

class GaduServersManager
{
	Q_DISABLE_COPY(GaduServersManager);

	static GaduServersManager * Instance;

	QList<int> Ports;
	int LastGoodPort;
	QList<QHostAddress> Servers;
	QHostAddress LastGoodServer;

	int LastPortTriedIndex;
	int LastServerTriedIndex;

	GaduServersManager();

public:
	static GaduServersManager * instance();

	QHostAddress getGoodServer();
	int getGoodPort() { return Ports[LastPortTriedIndex]; }

	void markServerAsGood(const QHostAddress &goodServer);
	void markPortAsGood(int port);

};

#endif // #ifndef GADU_SERVERS_MANAGER_H
