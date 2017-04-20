/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-contact-avatar-service.h"
#include "facebook-contact-avatar-service.moc"

#include "qfacebook/qfacebook-contact.h"

#include "avatars/contact-avatar-id.h"
#include "avatars/http-avatar-downloader.h"
#include "contacts/contact-id.h"

FacebookContactAvatarService::FacebookContactAvatarService(Account account, QObject *parent)
        : ContactAvatarService{account, parent}
{
}

FacebookContactAvatarService::~FacebookContactAvatarService() = default;

void FacebookContactAvatarService::download(const ContactAvatarId &id)
{
    auto avatarDownloader = make_owned<HttpAvatarDownloader>(id, id.id, this);
    connect(avatarDownloader, &HttpAvatarDownloader::downloaded, this, &FacebookContactAvatarService::downloaded);
}

void FacebookContactAvatarService::contactAdded(const QFacebookContact &c)
{
    emit available({{c.id()}, c.avatarUrl()});
}
