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

#include "configuration_aware_object.h"

class GaduServersManager : public ConfigurationAwareObject
{
	Q_DISABLE_COPY(GaduServersManager);

public:
	typedef QPair<QHostAddress, int> GaduServer;

private:
	static const char *Ips[];
	static GaduServersManager * Instance;

	QList<QHostAddress> AllServers;
	QList<int> AllPorts;
	QList<GaduServer> GoodServers;
	QList<GaduServer> BadServers;

	GaduServersManager();

	void buildServerList();

protected:
	virtual void configurationUpdated();

public:
	static GaduServersManager * instance();

	QPair<QHostAddress, int> getServer();
	void markServerAsGood(GaduServer server);
	void markServerAsBad(GaduServer server);

};

#endif // #ifndef GADU_SERVERS_MANAGER_H
