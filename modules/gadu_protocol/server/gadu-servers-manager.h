/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_SERVERS_MANAGER_H
#define GADU_SERVERS_MANAGER_H

#include <QtCore/QList>
#include <QtNetwork/QHostAddress>

#include "configuration/configuration-aware-object.h"

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
