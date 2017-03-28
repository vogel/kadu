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

#include "qfacebook-message-type.h"
#include "qfacebook-message.h"

#include <QtCore/QByteArray>

struct QMqttMessage;

class QFacebookConnect : public QFacebookMessage
{
public:
    virtual QFacebookMessageType messageType() const
    {
        return QFacebookMessageType::Connect;
    }
    virtual QMqttMessage encode() const;

    QByteArray cid;
    int64_t uid;
    QByteArray information;
    int64_t unknownCp{23};
    int64_t unknownEcp{26};
    int32_t unknown{1};
    bool noAutoFg{true};
    bool visible{false};
    QByteArray did;
    bool unknownFg{true};
    int32_t unknownNwt{1};
    int32_t unknownNwst{0};
    int64_t mid;
    QByteArray token;
    uint16_t keepAlive;
};
