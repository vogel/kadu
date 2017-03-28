/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtNetwork/QHostAddress>
#include <injeqt/injeqt.h>

#include "../gadu-exports.h"

class GADUAPI GaduServersManager : public QObject
{
    Q_OBJECT

public:
    typedef QPair<QHostAddress, int> GaduServer;

    Q_INVOKABLE explicit GaduServersManager(QObject *parent = nullptr);
    virtual ~GaduServersManager();

    QPair<QHostAddress, int> getServer();
    void markServerAsBad(GaduServer server);

private:
    QList<GaduServer> AllServers;
    QList<GaduServer> GoodServers;
    QList<GaduServer> BadServers;

private slots:
    INJEQT_INIT void init();
};
