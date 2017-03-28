/*
 * %kadu copyright begin%
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

#pragma once

#include "model/kadu-abstract-model.h"
#include "network/proxy/network-proxy.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>

class NetworkProxyManager;

class NetworkProxyModel : public QAbstractListModel, public KaduAbstractModel
{
    Q_OBJECT

public:
    explicit NetworkProxyModel(QObject *parent = nullptr);
    virtual ~NetworkProxyModel();

    virtual int columnCount(const QModelIndex &parent) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    NetworkProxy networkProxy(const QModelIndex &index) const;
    int networkProxyIndex(NetworkProxy networkProxy) const;
    virtual QModelIndexList indexListForValue(const QVariant &value) const;

private:
    QPointer<NetworkProxyManager> m_networkProxyManager;

private slots:
    INJEQT_SET void setNetworkProxyManager(NetworkProxyManager *networkProxyManager);
    INJEQT_INIT void init();

    void networkProxyUpdated(NetworkProxy networkProxy);
    void networkProxyAboutToBeAdded(NetworkProxy networkProxy);
    void networkProxyAdded(NetworkProxy networkProxy);
    void networkProxyAboutToBeRemoved(NetworkProxy networkProxy);
    void networkProxyRemoved(NetworkProxy networkProxy);
};
