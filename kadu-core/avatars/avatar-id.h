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

#pragma once

#include "exports.h"

#include <QtCore/QString>
#include <vector>

class Buddy;
class Contact;
struct ContactGlobalId;

struct KADUAPI AvatarId
{
    QString value;
};

KADUAPI inline bool operator==(const AvatarId &x, const AvatarId &y)
{
    return x.value == y.value;
}

KADUAPI inline bool operator!=(const AvatarId &x, const AvatarId &y)
{
    return !(x == y);
}

KADUAPI inline bool operator<(const AvatarId &x, const AvatarId &y)
{
    return x.value < y.value;
}

KADUAPI AvatarId avatarId(const Buddy &buddy);
KADUAPI AvatarId avatarId(const Contact &contact);
KADUAPI AvatarId avatarId(const ContactGlobalId &contact);
KADUAPI std::vector<AvatarId> avatarIds(const Contact &contact);
