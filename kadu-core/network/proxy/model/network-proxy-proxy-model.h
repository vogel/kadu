/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NETWORK_PROXY_PROXY_MODEL_H
#define NETWORK_PROXY_PROXY_MODEL_H

#include <QtCore/QSortFilterProxyModel>

class NetworkProxyProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	bool BrokenStringCompare;
	int compareNames(const QString &n1, const QString &n2) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	NetworkProxyProxyModel(QObject *parent = 0);
	virtual ~NetworkProxyProxyModel();

};

#endif // NETWORK_PROXY_PROXY_MODEL_H
