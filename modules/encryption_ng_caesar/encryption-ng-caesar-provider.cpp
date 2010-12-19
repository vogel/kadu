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

#include <QtCore/QtGlobal>

#include "encryption-ng-caesar-decryptor.h"
#include "encryption-ng-caesar-encryptor.h"

#include "encryption-ng-caesar-provider.h"

EncryptionNgCaesarProvider * EncryptionNgCaesarProvider::Instance = 0;

void EncryptionNgCaesarProvider::createInstance()
{
    Instance = new EncryptionNgCaesarProvider();
}

void EncryptionNgCaesarProvider::destroyInstance()
{
    delete Instance;
    Instance = 0;
}

EncryptionNgCaesarProvider::EncryptionNgCaesarProvider()
{
    CaesarDecryptor = new EncryptionNgCaesarDecryptor();
    CaesarEncryptor = new EncryptionNgCaesarEncryptor();
}

EncryptionNgCaesarProvider::~EncryptionNgCaesarProvider()
{
    delete CaesarDecryptor;
    CaesarDecryptor = 0;
    delete CaesarEncryptor;
    CaesarEncryptor = 0;
}

bool EncryptionNgCaesarProvider::canDecrypt(const Chat &chat)
{
    Q_UNUSED(chat)

    return true;
}

bool EncryptionNgCaesarProvider::canEncrypt(const Chat &chat)
{
    Q_UNUSED(chat)

    return true;
}

Decryptor * EncryptionNgCaesarProvider::decryptor(const Chat &chat)
{
    Q_UNUSED(chat)

    return CaesarDecryptor;
}

Encryptor * EncryptionNgCaesarProvider::encryptor(const Chat &chat)
{
    Q_UNUSED(chat)

    return CaesarEncryptor;
}
