/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <libgadu.h>

#include "gadu-servers-manager.h"

GaduServersManager::GaduServersManager(QObject *parent) : QObject{parent}
{
}

GaduServersManager::~GaduServersManager()
{
}

void GaduServersManager::init()
{
    GoodServers << GaduServer(QHostAddress((quint32)0), 0);   // for GG hub
    for (auto i = 108; i <= 123; i++)
        GoodServers << GaduServer{QHostAddress{QString{"91.214.237.%1"}.arg(i)}, 443};
    AllServers = GoodServers;
}

GaduServersManager::GaduServer GaduServersManager::getServer()
{
    if (GoodServers.isEmpty())
    {
        GoodServers = BadServers;
        BadServers.clear();
        return GaduServer(QHostAddress(), 0);
    }

    if (GoodServers[0].second != 443 && GoodServers[0].second != 0)
    {
        markServerAsBad(GoodServers[0]);
        return getServer();
    }

    return GoodServers[0];
}

void GaduServersManager::markServerAsBad(GaduServersManager::GaduServer server)
{
    GoodServers.removeAll(server);
    BadServers.append(server);
}
