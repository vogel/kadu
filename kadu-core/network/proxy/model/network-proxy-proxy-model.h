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

#ifndef NETWORK_PROXY_PROXY_MODEL_H
#define NETWORK_PROXY_PROXY_MODEL_H

#include <QtGui/QSortFilterProxyModel>

class NetworkProxyProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	NetworkProxyProxyModel(QObject *parent = 0);
	virtual ~NetworkProxyProxyModel();

	virtual QModelIndex indexForValue(const QVariant &value) const;

};

#endif // NETWORK_PROXY_PROXY_MODEL_H
