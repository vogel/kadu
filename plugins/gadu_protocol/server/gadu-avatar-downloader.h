/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatars/contact-avatar-id.h"
#include "misc/memory.h"

#include <QtCore/QObject>

class QNetworkAccessManager;
class QNetworkReply;

class GaduAvatarDownloader : public QObject
{
    Q_OBJECT

public:
    explicit GaduAvatarDownloader(ContactAvatarId id, QObject *parent = nullptr);
    virtual ~GaduAvatarDownloader();

signals:
    void downloaded(const ContactAvatarId &id, const QByteArray &content);

private:
    owned_qptr<QNetworkAccessManager> m_nam;
    owned_qptr<QNetworkReply> m_reply;
    int m_redirectCount;

    ContactAvatarId m_id;

    void done(QByteArray avatar);
    void failed();

    void fetch(const QString &url);
    void parseReply();

private slots:
    void requestFinished();
};
