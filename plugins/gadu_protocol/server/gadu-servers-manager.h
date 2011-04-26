/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Ziemniak (jziemkiewicz@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "../gadu-exports.h"

#include "configuration/configuration-aware-object.h"

class GADUAPI GaduServersManager : public ConfigurationAwareObject
{
	Q_DISABLE_COPY(GaduServersManager)

public:
	typedef QPair<QHostAddress, int> GaduServer;

private:
	static GaduServersManager * Instance;

	QList<int> AllPorts;
	QList<GaduServer> AllServers;
	QList<GaduServer> GoodServers;
	QList<GaduServer> BadServers;

	GaduServersManager();

	QList<GaduServer> gaduServersFromString(const QString &serverAddress);

	void loadServerListFromFile(const QString &fileName);
	void loadServerListFromString(const QString &data);

protected:
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();

	static GaduServersManager * instance() { return Instance; }

	const QList<GaduServer> & getServersList();
	QPair<QHostAddress, int> getServer(bool onlyTls);
	void markServerAsGood(GaduServer server);
	void markServerAsBad(GaduServer server);

	void buildServerList();

};

#endif // #ifndef GADU_SERVERS_MANAGER_H
