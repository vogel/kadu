/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "jabber-avatar-downloader.h"
#include "jabber-avatar-downloader.moc"

#include "jabber-protocol.h"
#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"

#include <qxmpp/QXmppVCardIq.h>

JabberAvatarDownloader::JabberAvatarDownloader(ContactAvatarId id, JabberVCardService *vCardService, QObject *parent)
        : QObject{parent}, m_id{std::move(id)}, m_vCardService{vCardService}
{
    auto vCardDownloader = m_vCardService.data()->createVCardDownloader();
    if (!vCardDownloader)
    {
        failed();
        return;
    }

    connect(vCardDownloader, &JabberVCardDownloader::vCardDownloaded, this, &JabberAvatarDownloader::vCardDownloaded);
    vCardDownloader->downloadVCard(m_id.contact.value);
}

JabberAvatarDownloader::~JabberAvatarDownloader()
{
}

void JabberAvatarDownloader::done(QByteArray avatar)
{
    emit downloaded(m_id, std::move(avatar));
    deleteLater();
}

void JabberAvatarDownloader::failed()
{
    deleteLater();
}

void JabberAvatarDownloader::vCardDownloaded(bool ok, const QXmppVCardIq &vCard)
{
    if (ok)
        done(vCard.photo());
    else
        failed();
}
