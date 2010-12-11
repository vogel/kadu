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

#include "encryption-ng-ceasar-provider.h"

extern "C" int encryption_ng_ceasar_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	EncryptionNgCeasarProvider::createInstance();
	EncryptionProviderManager::instance()->registerProvider(EncryptionNgCeasarProvider::instance());

	return 0;
}

extern "C" void encryption_ng_ceasar_close()
{
	EncryptionProviderManager::instance()->unregisterProvider(EncryptionNgCeasarProvider::instance());
	EncryptionNgCeasarProvider::destroyInstance();
}
