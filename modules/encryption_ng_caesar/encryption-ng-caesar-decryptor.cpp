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

#include "encryption-ng-caesar-decryptor.h"

QByteArray EncryptionNgCaesarDecryptor::decrypt(const QByteArray &data)
{
    if (!data.startsWith(KADU_CAESAR_ENCRYPTION_MARKER_BEGIN) || !data.endsWith(KADU_CAESAR_ENCRYPTION_MARKER_END))
        return data;

    QByteArray base64 = data.mid(strlen(KADU_CAESAR_ENCRYPTION_MARKER_BEGIN),
            data.length() - strlen(KADU_CAESAR_ENCRYPTION_MARKER_BEGIN) - strlen(KADU_CAESAR_ENCRYPTION_MARKER_END));

    QCA::Base64 decoder;
    QByteArray encryped = decoder.decode(base64).toByteArray();

    QByteArray result;
    for (int i = 0, s = encryped.size(); i < s; i++)
        result.append(encryped.at(i) - 1);

    return result;
}
