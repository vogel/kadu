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

#include "gadu-contact-avatar-service.h"
#include "gadu-contact-avatar-service.moc"

#include "contacts/contact-id.h"

GaduContactAvatarService::GaduContactAvatarService(Account account, QObject *parent)
        : ContactAvatarService{account, parent}
{
}

GaduContactAvatarService::~GaduContactAvatarService() = default;

void GaduContactAvatarService::downloadAvatar(const ContactId &contactId, const QByteArray &id)
{
    (void)contactId;
    (void)id;
}

void GaduContactAvatarService::handleAvatarData(const uin_t uin, const struct gg_event_user_data_attr *const avatarData)
{
    auto contactId = ContactId{QByteArray::number(uin)};
    if (!avatarData || avatarData->type == 0)
    {
        emit avatarRemoved(contactId);
        return;
    }

    auto ok = false;
    auto timestamp = QString::fromLatin1(avatarData->value).toInt(&ok);

    if (!ok || timestamp <= 0)
    {
        emit avatarRemoved(contactId);
        return;
    }

    emit avatarAvailable(contactId, avatarData->value);
}
