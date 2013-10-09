/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/roles.h"
#include "network/proxy/network-proxy-manager.h"
#include "network/proxy/network-proxy.h"

#include "network-proxy-model.h"

NetworkProxyModel::NetworkProxyModel(QObject *parent) :
		QAbstractListModel(parent)
{
	connect(NetworkProxyManager::instance(), SIGNAL(networkProxyUpdated(NetworkProxy)),
			this, SLOT(networkProxyUpdated(NetworkProxy)), Qt::DirectConnection);
	connect(NetworkProxyManager::instance(), SIGNAL(networkProxyAboutToBeAdded(NetworkProxy)),
			this, SLOT(networkProxyAboutToBeAdded(NetworkProxy)), Qt::DirectConnection);
	connect(NetworkProxyManager::instance(), SIGNAL(networkProxyAdded(NetworkProxy)),
			this, SLOT(networkProxyAdded(NetworkProxy)), Qt::DirectConnection);
	connect(NetworkProxyManager::instance(), SIGNAL(networkProxyAboutToBeRemoved(NetworkProxy)),
			this, SLOT(networkProxyAboutToBeRemoved(NetworkProxy)), Qt::DirectConnection);
	connect(NetworkProxyManager::instance(), SIGNAL(networkProxyRemoved(NetworkProxy)),
			this, SLOT(networkProxyRemoved(NetworkProxy)), Qt::DirectConnection);
}

NetworkProxyModel::~NetworkProxyModel()
{
	disconnect(NetworkProxyManager::instance(), 0, this, 0);
}

int NetworkProxyModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int NetworkProxyModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : NetworkProxyManager::instance()->count();
}

QVariant NetworkProxyModel::data(const QModelIndex &index, int role) const
{
	NetworkProxy proxy = networkProxy(index);
	if (proxy.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return proxy.displayName();

		case NetworkProxyRole:
			return QVariant::fromValue<NetworkProxy>(proxy);

		case ItemTypeRole:
			return NetworkProxyRole;

		default:
			return QVariant();
	}
}

NetworkProxy NetworkProxyModel::networkProxy(const QModelIndex &index) const
{
	if (!index.isValid())
		return NetworkProxy::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return NetworkProxy::null;

	return NetworkProxyManager::instance()->byIndex(index.row());
}

int NetworkProxyModel::networkProxyIndex(NetworkProxy networkProxy) const
{
	return NetworkProxyManager::instance()->indexOf(networkProxy);
}

QModelIndexList NetworkProxyModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const int i = networkProxyIndex(value.value<NetworkProxy>());
	if (-1 != i)
		result.append(index(i, 0));

	return result;
}

void NetworkProxyModel::networkProxyUpdated(NetworkProxy networkProxy)
{
	const QModelIndexList &indexes = indexListForValue(networkProxy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	emit dataChanged(index, index);
}

void NetworkProxyModel::networkProxyAboutToBeAdded(NetworkProxy networkProxy)
{
	Q_UNUSED(networkProxy)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void NetworkProxyModel::networkProxyAdded(NetworkProxy networkProxy)
{
	Q_UNUSED(networkProxy)

	endInsertRows();
}

void NetworkProxyModel::networkProxyAboutToBeRemoved(NetworkProxy networkProxy)
{
	int index = networkProxyIndex(networkProxy);
	beginRemoveRows(QModelIndex(), index, index);
}

void NetworkProxyModel::networkProxyRemoved(NetworkProxy networkProxy)
{
	Q_UNUSED(networkProxy)

	endRemoveRows();
}

#include "moc_network-proxy-model.cpp"
