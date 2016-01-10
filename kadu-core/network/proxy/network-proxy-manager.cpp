/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtXml/QDomElement>

#include "configuration/configuration-api.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "storage/storage-point.h"

#include "network-proxy-manager.h"

NetworkProxyManager::NetworkProxyManager(QObject *parent) :
		QObject{parent}
{
}

NetworkProxyManager::~NetworkProxyManager()
{
}

void NetworkProxyManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void NetworkProxyManager::init()
{
	m_configurationManager->registerStorableObject(this);
	configurationUpdated();
}

void NetworkProxyManager::done()
{
	m_configurationManager->unregisterStorableObject(this);
}

void NetworkProxyManager::load()
{
	QMutexLocker locker(&mutex());

	SimpleManager<NetworkProxy>::load();
}

void NetworkProxyManager::store()
{
	QMutexLocker locker(&mutex());

	SimpleManager<NetworkProxy>::store();
}

void NetworkProxyManager::configurationUpdated()
{
	DefaultProxy = byUuid(Core::instance()->configuration()->deprecatedApi()->readEntry("Network", "DefaultProxy"));
}

void NetworkProxyManager::setDefaultProxy(const NetworkProxy &proxy)
{
	DefaultProxy = proxy;
	Core::instance()->configuration()->deprecatedApi()->writeEntry("Network", "DefaultProxy", DefaultProxy.uuid().toString());
}

const NetworkProxy & NetworkProxyManager::defaultProxy()
{
	return DefaultProxy;
}

NetworkProxy NetworkProxyManager::byConfiguration(const QString &address, int port,
                                                  const QString &user, const QString &password, NotFoundAction action)
{
	foreach (const NetworkProxy &networkProxy, items())
	{
		if (networkProxy.address() == address &&
		       networkProxy.port() == port &&
		       networkProxy.user() == user &&
		       networkProxy.password() == password)
			return  networkProxy;
	}

	if (ActionReturnNull == action)
		return NetworkProxy::null;

	NetworkProxy networkProxy = NetworkProxy::create();
	networkProxy.setAddress(address);
	networkProxy.setPort(port);
	networkProxy.setUser(user);
	networkProxy.setPassword(password);

	if (ActionCreateAndAdd == action)
		addItem(networkProxy);

	return networkProxy;
}

void NetworkProxyManager::networkProxyDataUpdated()
{
	NetworkProxy networkProxy(sender());
	if (!networkProxy.isNull())
		emit networkProxyUpdated(networkProxy);
}

void NetworkProxyManager::itemAboutToBeAdded(NetworkProxy item)
{
	connect(item, SIGNAL(updated()), this, SLOT(networkProxyDataUpdated()));
	emit networkProxyAboutToBeAdded(item);
}

void NetworkProxyManager::itemAdded(NetworkProxy item)
{
	emit networkProxyAdded(item);
}

void NetworkProxyManager::itemAboutToBeRemoved(NetworkProxy item)
{
	emit networkProxyAboutToBeRemoved(item);
}

void NetworkProxyManager::itemRemoved(NetworkProxy item)
{
	disconnect(item, 0, this, 0);
	emit networkProxyRemoved(item);
}

#include "moc_network-proxy-manager.cpp"
