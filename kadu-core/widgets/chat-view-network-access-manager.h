/*
 * %kadu copyright begin%
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <injeqt/injeqt.h>

class ImageStorageService;

// taken from "Adding New Protocols to QtWebKit" article found in Qt Quarterly
// http://doc.trolltech.com/qq/32/qq32-webkit-protocols.html
class ChatViewNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

    QPointer<ImageStorageService> CurrentImageStorageService;

public:
    explicit ChatViewNetworkAccessManager(QNetworkAccessManager *oldManager, QObject *parent = nullptr);
    virtual ~ChatViewNetworkAccessManager();

protected:
    virtual QNetworkReply *
    createRequest(Operation operation, const QNetworkRequest &request, QIODevice *device) override;

private slots:
    INJEQT_SET void setImageStorageService(ImageStorageService *imageStorageService);
};
