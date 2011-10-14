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

#ifndef NETWORK_PROXY_MANAGER_H
#define NETWORK_PROXY_MANAGER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "configuration/configuration-aware-object.h"
#include "network/proxy/network-proxy.h"
#include "storage/simple-manager.h"

#include "exports.h"

class NetworkProxy;

class KADUAPI NetworkProxyManager : public QObject, public SimpleManager<NetworkProxy>, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NetworkProxyManager)

	static NetworkProxyManager *Instance;

	NetworkProxy DefaultProxy;

	NetworkProxyManager();
	virtual ~NetworkProxyManager();

private slots:
	void networkProxyDataUpdated();

protected:
	virtual void load();
	virtual void store();

	virtual void itemAboutToBeAdded(NetworkProxy item);
	virtual void itemAdded(NetworkProxy item);
	virtual void itemAboutToBeRemoved(NetworkProxy item);
	virtual void itemRemoved(NetworkProxy item);

	virtual void configurationUpdated();

public:
	static NetworkProxyManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Proxys"); }
	virtual QString storageNodeItemName() { return QLatin1String("Proxy"); }

	void setDefaultProxy(const NetworkProxy &proxy);
	const NetworkProxy &defaultProxy();

	NetworkProxy byConfiguration(const QString &address, int port,
	                             const QString &user, const QString &password, NotFoundAction action);

signals:
	void networkProxyAboutToBeAdded(NetworkProxy networkProxy);
	void networkProxyAdded(NetworkProxy networkProxy);
	void networkProxyAboutToBeRemoved(NetworkProxy networkProxy);
	void networkProxyRemoved(NetworkProxy networkProxy);

	void networkProxyUpdated(NetworkProxy networkProxy);

};

#include "network/proxy/network-proxy.h" // for MOC

#endif // NETWORK_PROXY_MANAGER_H
