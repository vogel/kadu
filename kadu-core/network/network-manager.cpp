/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "network/network-aware-object.h"
#include "kadu-network-config.h"

#include "network-manager.h"

#include NETWORK_IMPLEMENTATION_INCLUDE

NetworkManager *NetworkManager::Instance = 0;

/**
 * @author Rafał 'Vogel' Malinowski
 * @author Piotr 'ultr' Dąbrowski
 * @short Returns singleton instance of NetworkManager.
 * @return singleton instance of NetworkManager
 *
 * This static method returns singleton instance of NetworkManager. Implementation class is selected at compile
 * time and can be of one of the following classes:
 * <ul>
 *   <li>NetworkManagerDummy</li>
 *   <li>NetworkManagerNTrack</li>
 *   <li>NetworkManagerQt</li>
 * </ul>
 */
NetworkManager * NetworkManager::instance()
{
	if (!Instance)
		Instance = new NETWORK_IMPLEMENTATION_CLASS_NAME();

	return Instance;
}

NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Implementations should call this method each time network state changes.
 * @param isOnline new network state
 *
 * Implementations should call this method each time network state changes. This method will then inform
 * all NetworkAwareObject's about the change and emit one of online() or offline() signals.
 */
void NetworkManager::onlineStateChanged(bool isOnline)
{
	NetworkAwareObject::notifyOnlineStateChanged(isOnline);
	if (isOnline)
		emit online();
	else
		emit offline();
}

#include "moc_network-manager.cpp"
