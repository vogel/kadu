/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QSysInfo>
#include <QtNetwork/QNetworkConfigurationManager>

#include "network-manager-qt.h"

NetworkManagerQt::NetworkManagerQt()
{
#ifdef Q_OS_WIN
	// Kadu bug #2591
	if (QSysInfo::WindowsVersion < QSysInfo::WV_VISTA)
	{
		ConfigurationManager = 0;
		HasValidCapabilities = false;
	}
	else
#endif
	{
		ConfigurationManager = new QNetworkConfigurationManager(this);
		HasValidCapabilities = ConfigurationManager->capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces;

		if (HasValidCapabilities)
			connect(ConfigurationManager, SIGNAL(onlineStateChanged(bool)), this, SLOT(onlineStateChanged(bool)));
	}
}

NetworkManagerQt::~NetworkManagerQt()
{
}

bool NetworkManagerQt::isOnline()
{
	return HasValidCapabilities
			? ConfigurationManager->isOnline()
			: true;
}

void NetworkManagerQt::forceOnline()
{
	onlineStateChanged(true);
}

#include "moc_network-manager-qt.cpp"
