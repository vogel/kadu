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

#include "avatar-id.h"

#include "buddies/buddy.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact.h"

AvatarId avatarId(const Buddy &buddy)
{
    if (buddy.hasProperty("avatar:path"))
        return AvatarId{buddy.property("avatar:path", QString{}).toString()};
    else
        return AvatarId{buddy.uuid().toString()};
}

AvatarId avatarId(const Contact &contact)
{
    auto id = ContactGlobalId{contact.contactAccount(), {contact.id().toUtf8()}};
    return avatarId(id);
}

AvatarId avatarId(const ContactGlobalId &contact)
{
    auto uniqueString = QString{"%1-%2-%3"}.arg(
        contact.account.protocolName(), contact.account.id(), QString::fromUtf8(contact.id.value));
    return AvatarId{QString::number(qHash(uniqueString))};
}

std::vector<AvatarId> avatarIds(const Contact &contact)
{
    return {avatarId(contact.ownerBuddy()), avatarId(contact)};
}
