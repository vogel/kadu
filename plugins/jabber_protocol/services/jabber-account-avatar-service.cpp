/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "jabber-account-avatar-service.h"
#include "jabber-account-avatar-service.moc"

#include "jabber-protocol.h"
#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "services/jabber-vcard-uploader.h"

#include "avatars/avatars.h"

#include <QXmppVCardIq.h>
#include <QtCore/QBuffer>

namespace
{
QByteArray avatarData(QPixmap avatar)
{
    avatar = avatar.scaled(AVATAR_SIZE, AVATAR_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    avatar.save(&buffer, "PNG");
    buffer.close();

    return data;
}
}

JabberAccountAvatarService::JabberAccountAvatarService(Account account, QObject *parent)
        : AccountAvatarService{account, parent}
{
}

JabberAccountAvatarService::~JabberAccountAvatarService()
{
}

void JabberAccountAvatarService::setVCardService(JabberVCardService *vCardService)
{
    m_vCardService = vCardService;
}

void JabberAccountAvatarService::upload(const QPixmap &avatar)
{
    if (!m_vCardService)
    {
        emit finished(false);
        return;
    }

    auto vCardDownloader = m_vCardService.data()->createVCardDownloader();
    if (!vCardDownloader)
    {
        emit finished(false);
        return;
    }

    m_avatar = avatar;
    connect(
        vCardDownloader, &JabberVCardDownloader::vCardDownloaded, this, &JabberAccountAvatarService::vCardDownloaded);
    vCardDownloader->downloadVCard(account().id());
}

void JabberAccountAvatarService::vCardDownloaded(bool ok, const QXmppVCardIq &vcard)
{
    if (!ok || !m_vCardService)
    {
        emit finished(false);
        return;
    }

    auto updatedVCard = vcard;
    updatedVCard.setPhoto(avatarData(m_avatar));

    auto vCardUploader = m_vCardService.data()->createVCardUploader();
    if (!vCardUploader)
    {
        emit finished(false);
        return;
    }

    connect(vCardUploader, &JabberVCardUploader::vCardUploaded, this, &JabberAccountAvatarService::vCardUploaded);
    vCardUploader->uploadVCard(updatedVCard);
}

void JabberAccountAvatarService::vCardUploaded(bool ok)
{
    emit finished(ok);
}
