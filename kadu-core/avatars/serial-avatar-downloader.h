/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class AggregatedContactAvatarService;
struct ContactAvatarGlobalId;

class KADUAPI SerialAvatarDownloader : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SerialAvatarDownloader(QObject *parent = nullptr);
    virtual ~SerialAvatarDownloader();

    void downloadAvatar(const ContactAvatarGlobalId &id);

private:
    QPointer<AggregatedContactAvatarService> m_aggregatedContactAvatarService;

    std::vector<ContactAvatarGlobalId> m_avatarsToDownload;
    QTimer m_timer;

private slots:
    INJEQT_SET void setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService);

    void downloadNextAvatar();
};
