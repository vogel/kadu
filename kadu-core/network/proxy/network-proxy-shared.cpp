/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/injected-factory.h"
#include "misc/change-notifier.h"
#include "network/proxy/network-proxy-manager.h"

#include "network-proxy-shared.h"
#include "network-proxy-shared.moc"

NetworkProxyShared::NetworkProxyShared(const QUuid &uuid) : Shared(uuid), Port(0)
{
    connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

NetworkProxyShared::~NetworkProxyShared()
{
    ref.ref();
}

void NetworkProxyShared::setNetworkProxyManager(NetworkProxyManager *networkProxyManager)
{
    m_networkProxyManager = networkProxyManager;
}

StorableObject *NetworkProxyShared::storageParent()
{
    return m_networkProxyManager;
}

QString NetworkProxyShared::storageNodeName()
{
    return QStringLiteral("Proxy");
}

void NetworkProxyShared::load()
{
    if (!isValidStorage())
        return;

    Shared::load();

    Type = loadValue<QString>("Type");
    if (Type.isEmpty())
        Type = "http";

    Address = loadValue<QString>("Address");
    Port = loadValue<int>("Port", 0);
    User = loadValue<QString>("User");
    Password = loadValue<QString>("Password");
    PollingUrl = loadValue<QString>("PollingUrl");
}

void NetworkProxyShared::store()
{
    if (!isValidStorage())
        return;

    ensureLoaded();

    Shared::store();

    storeValue("Type", Type);
    storeValue("Address", Address);
    storeValue("Port", Port);
    storeValue("User", User);
    storeValue("Password", Password);
    storeValue("PollingUrl", PollingUrl);
}

QString NetworkProxyShared::displayName()
{
    ensureLoaded();

    if (User.isEmpty())
        return Address + ":" + QString::number(Port);
    else
        return User + "@" + Address + ":" + QString::number(Port);
}
