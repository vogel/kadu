/*
 * %kadu copyright begin%
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

#pragma once

#include "configuration/configuration-aware-object.h"
#include "network/proxy/network-proxy.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <injeqt/injeqt.h>

class ConfigurationManager;
class Configuration;
class NetworkProxyStorage;
class NetworkProxy;

class KADUAPI NetworkProxyManager : public SimpleManager<NetworkProxy>, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit NetworkProxyManager(QObject *parent = nullptr);
	virtual ~NetworkProxyManager();

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

protected:
	virtual void load();
	virtual void store();
	virtual NetworkProxy loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);

	virtual void itemAboutToBeAdded(NetworkProxy item);
	virtual void itemAdded(NetworkProxy item);
	virtual void itemAboutToBeRemoved(NetworkProxy item);
	virtual void itemRemoved(NetworkProxy item);

	virtual void configurationUpdated();

private:
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;
	QPointer<NetworkProxyStorage> m_networkProxyStorage;

	NetworkProxy DefaultProxy;

private slots:
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setNetworkProxyStorage(NetworkProxyStorage *networkProxyStorage);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void networkProxyDataUpdated();

};
