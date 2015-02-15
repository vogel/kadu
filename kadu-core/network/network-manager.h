/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QtCore/QObject>

#include "exports.h"

/**
 * @addtogroup Network
 * @{
 */

/**
 * @class NetworkManager
 * @author Rafał 'Vogel' Malinowski
 * @author Piotr 'ultr' Dąbrowski
 * @short Class responsible for network online-offline notifications.
 * @see NetworkAwareObject
 *
 * This class provides information about network availability. To check if network is available use isOnline()
 * method. Any object can connect to online() and offline() signals to get real-time notifications.
 *
 * On some systems that do not provide network availability information this class can always return online status.
 *
 * This class is singleton with possible different implementations, see NetworkManagerDummy, NetworkManagerNTrack
 * and NetworkManagerQt for current implementation details.
 */
class KADUAPI NetworkManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NetworkManager)

	static NetworkManager *Instance;

protected slots:
	void onlineStateChanged(bool isOnline);

protected:
	NetworkManager();
	virtual ~NetworkManager();

public:
	static NetworkManager * instance();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @author Piotr 'ultr' Dąbrowski
	 * @short This method returns true if netwok is online.
	 * @return true if netwok is online
	 *
	 * This method returns true if netwok is online. On some systems that do not provide network availability information
	 * this method can always return true.
	 */
	virtual bool isOnline() = 0;

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Force online status on network.
	 *
	 * If implementation of network manager is broken (as in ntrack) call this method if you are sure that network is online.
	 */
	virtual void forceOnline() = 0;

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when network changes state from offline to online.
	 *
	 * This signal is emited when network changes state from offline to online. On some systems that do not provide network
	 * availability information this signal will be never emited.
	 */
	void online();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when network changes state from online to offline.
	 *
	 * This signal is emited when network changes state from online to offline. On some systems that do not provide network
	 * availability information this signal will be never emited.
	 */
	void offline();

};

/**
 * @}
 */

#endif // NETWORK_MANAGER_H
