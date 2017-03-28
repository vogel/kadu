/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Based on bitlbee-facebook by James Geboski (jgeboski@gmail.com) and
 * dequis <dx@dxzone.com.ar>.
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

#include "qfacebook-publish-presence.h"

#include "qfacebook/exceptions/qfacebook-invalid-data-exception.h"
#include "qthrift/qthrift-exception.h"
#include "qthrift/qthrift-reader.h"

#include <cstdint>

namespace
{
std::experimental::optional<QThrift::List<QThrift::Struct>> readPresenceList(const QByteArray &data)
{
    try
    {
        auto thriftReader = QThrift::Reader(data);
        thriftReader.readString();
        auto presenceStruct = thriftReader.readValue<QThrift::Struct>();
        return presenceStruct.get<QThrift::List<QThrift::Struct>>(2);
    }
    catch (QThrift::Exception &)
    {
        throw QFacebookInvalidDataException{};
    }
}
}

QFacebookPublishPresence QFacebookPublishPresence::decode(const QByteArray &data)
{
    auto presenceList = readPresenceList(data);
    if (!presenceList)
        throw QFacebookInvalidDataException{};

    auto result = std::vector<QFacebookPresence>{};
    for (auto const &presenceItem : *presenceList)
    {
        auto id = presenceItem.get<int64_t>(1);
        auto active = presenceItem.get<int32_t>(2);
        if (!id || !active)
            throw QFacebookInvalidDataException{};

        result.push_back(
            QFacebookPresence{*id, active == 0 ? QFacebookPresenceType::Offline : QFacebookPresenceType::Online});
    }

    return {result};
}
