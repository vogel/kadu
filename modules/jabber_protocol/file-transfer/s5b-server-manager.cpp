/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "s5b-server-manager.h"

S5BServerManager *S5BServerManager::Instance = 0;

void S5BServerManager::createInstance()
{
	if (!Instance)
		Instance = new S5BServerManager();
}

void S5BServerManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

S5BServerManager::S5BServerManager() :
		Server(0)
{
	setPort(8010);
}

S5BServerManager::~S5BServerManager()
{
}

void S5BServerManager::serverDestroyed()
{
	Server = 0;
	emit serverChanged(Server);
}

XMPP::S5BServer * S5BServerManager::server()
{
	if (!Server)
	{
		Server = new XMPP::S5BServer();
		connect(Server, SIGNAL(destroyed()), this, SLOT(serverDestroyed()));

		// TODO: remove this, we should not start a server in getter!!
		Server->start(Port);

		emit serverChanged(Server);
	}

	return Server;
}

void S5BServerManager::addAddress(const QString &address)
{
	Addresses.append(address);

	// remove duplicated
	server()->setHostList(QSet<QString>::fromList(Addresses).toList());
}

void S5BServerManager::removeAddress(const QString &address)
{
	Addresses.removeOne(address);
	if (Addresses.isEmpty())
	{
		delete Server;
		Server = 0;
	}
	else
		// remove duplicated
		server()->setHostList(QSet<QString>::fromList(Addresses).toList());
}

bool S5BServerManager::setPort(quint16 port)
{
	if (Port != port || !server()->isActive())
	{
		Port = port;
		return server()->start(Port);
	}

	return true;
}
