/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>

class JabberVCardService;

class QXmppVCardIq;

class JabberAvatarDownloader : public QObject
{
    Q_OBJECT

public:
    explicit JabberAvatarDownloader(ContactAvatarId id, JabberVCardService *vCardService, QObject *parent = nullptr);
    virtual ~JabberAvatarDownloader();

signals:
    void downloaded(const ContactAvatarId &id, const QByteArray &content);

private:
    ContactAvatarId m_id;
    QPointer<JabberVCardService> m_vCardService;

    void done(QByteArray avatar);
    void failed();

private slots:
    void vCardDownloaded(bool ok, const QXmppVCardIq &vCard);
};
