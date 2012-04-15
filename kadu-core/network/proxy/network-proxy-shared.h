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

#ifndef NETWORK_PROXY_SHARED_H
#define NETWORK_PROXY_SHARED_H

#include <QtNetwork/QHostAddress>

#include "exports.h"

#include "storage/shared.h"

class KADUAPI NetworkProxyShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(NetworkProxyShared)

	QString Type;
	QString Address;
	int Port;
	QString User;
	QString Password;
	QString PollingUrl;

protected:
	virtual void load();
	virtual void store();

public:
	static NetworkProxyShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &networkProxyStoragePoint);
	static NetworkProxyShared * loadFromStorage(const QSharedPointer<StoragePoint> &networkProxyStoragePoint);

	explicit NetworkProxyShared(const QUuid &uuid = QUuid());
	virtual ~NetworkProxyShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	KaduShared_Property(const QString &, type, Type)
	KaduShared_Property(const QString &, address, Address)
	KaduShared_Property(int, port, Port)
	KaduShared_Property(const QString &, user, User)
	KaduShared_Property(const QString &, password, Password)
	KaduShared_Property(const QString &, pollingUrl, PollingUrl)

	QString displayName();

signals:
	void updated();

};

#endif // NETWORK_PROXY_SHARED_H
