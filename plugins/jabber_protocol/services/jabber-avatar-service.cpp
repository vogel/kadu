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

#include "jabber-avatar-service.h"
#include "jabber-avatar-service.moc"

#include "jabber-protocol.h"
#include "services/jabber-avatar-downloader.h"
#include "services/jabber-avatar-uploader.h"
#include "services/jabber-vcard-service.h"

JabberAvatarService::JabberAvatarService(Account account, QObject *parent) : AvatarService{account, parent}
{
}

JabberAvatarService::~JabberAvatarService()
{
}

void JabberAvatarService::setVCardService(JabberVCardService *vCardService)
{
    VCardService = vCardService;
}

AvatarUploader *JabberAvatarService::createAvatarUploader()
{
    auto protocol = qobject_cast<JabberProtocol *>(account().protocolHandler());
    if (!protocol->isConnected())
        return nullptr;
    return new JabberAvatarUploader{VCardService.data(), this};
}
