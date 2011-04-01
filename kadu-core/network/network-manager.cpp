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

#include <QtNetwork/QNetworkConfigurationManager>

#include "network-manager.h"
#include "network-aware-object.h"

NetworkManager *NetworkManager::Instance = 0;

NetworkManager * NetworkManager::instance()
{
	if (!Instance)
		Instance = new NetworkManager();
	return Instance;
}

NetworkManager::NetworkManager()
{
	ConfigurationManager = new QNetworkConfigurationManager(this);
	connect(ConfigurationManager, SIGNAL(onlineStateChanged(bool)), this, SLOT(onlineStateChanged(bool)));
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::onlineStateChanged(bool isOnline)
{
	NetworkAwareObject::notifyOnlineStateChanged(isOnline);
}

bool NetworkManager::isOnline()
{
	return ConfigurationManager->isOnline();
}
