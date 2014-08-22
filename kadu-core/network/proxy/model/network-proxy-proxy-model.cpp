/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/network-proxy.h"

#include "network-proxy-proxy-model.h"

NetworkProxyProxyModel::NetworkProxyProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

NetworkProxyProxyModel::~NetworkProxyProxyModel()
{
}

int NetworkProxyProxyModel::compareNames(const QString &n1, const QString &n2) const
{
	return BrokenStringCompare
			? n1.toLower().localeAwareCompare(n2.toLower())
			: n1.localeAwareCompare(n2);
}

bool NetworkProxyProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	NetworkProxy leftNetworkProxy = left.data(NetworkProxyRole).value<NetworkProxy>();
	NetworkProxy rightNetworkProxy = right.data(NetworkProxyRole).value<NetworkProxy>();

	if (leftNetworkProxy.isNull())
		return false;
	if (rightNetworkProxy.isNull())
		return true;

	int displayCompare = compareNames(leftNetworkProxy.address(), rightNetworkProxy.address());
	return displayCompare < 0;
}

#include "moc_network-proxy-proxy-model.cpp"
