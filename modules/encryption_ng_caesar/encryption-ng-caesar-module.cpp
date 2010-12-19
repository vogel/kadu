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

#include "modules/encryption_ng/encryption-provider-manager.h"

#include "encryption-ng-caesar-provider.h"

extern "C" int encryption_ng_caesar_init(bool firstLoad)
{
    Q_UNUSED(firstLoad)

    EncryptionNgCaesarProvider::createInstance();
    EncryptionProviderManager::instance()->registerProvider(EncryptionNgCaesarProvider::instance());

    return 0;
}

extern "C" void encryption_ng_caesar_close()
{
    EncryptionProviderManager::instance()->unregisterProvider(EncryptionNgCaesarProvider::instance());
    EncryptionNgCaesarProvider::destroyInstance();
}
