/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QByteArray>
#include <QtCrypto>

#include "encryption-ng-caesar-marker.h"

#include "encryption-ng-caesar-encryptor.h"

QByteArray EncryptionNgCaesarEncryptor::encrypt(const QByteArray &data)
{
    QByteArray result;
    QCA::Base64 encoder;

    for (int i = 0, s = data.size(); i < s; i++)
        result.append(data.at(i) + 1);

    QByteArray encoded(KADU_CAESAR_ENCRYPTION_MARKER_BEGIN);
    encoded.append(encoder.encode(result).toByteArray());
    encoded.append(KADU_CAESAR_ENCRYPTION_MARKER_END);

    return encoded;
}
