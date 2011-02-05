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

#include "modules/encryption_ng/encryption-manager.h"
#include "modules/encryption_ng/encryption-provider-manager.h"
#include "exports.h"

#include "encryption-ng-simlite-key-generator.h"
#include "encryption-ng-simlite-key-importer.h"
#include "encryption-ng-simlite-provider.h"

extern "C" KADU_EXPORT int encryption_ng_simlite_init(bool firstLoad)
{
	if (firstLoad)
		EncryptioNgSimliteKeyImporter::createInstance();

	EncryptioNgSimliteKeyGenerator::createInstance();
	EncryptionManager::instance()->setGenerator(EncryptioNgSimliteKeyGenerator::instance());

	EncryptioNgSimliteProvider::createInstance();
	EncryptionProviderManager::instance()->registerProvider(EncryptioNgSimliteProvider::instance());

	return 0;
}

extern "C" KADU_EXPORT void encryption_ng_simlite_close()
{
	EncryptionProviderManager::instance()->unregisterProvider(EncryptioNgSimliteProvider::instance());
	EncryptioNgSimliteProvider::destroyInstance();

	EncryptionManager::instance()->setGenerator(0);
	EncryptioNgSimliteKeyGenerator::destroyInstance();

	// it can work without createInstance too, so don't care about firstLoad here
	EncryptioNgSimliteKeyImporter::destroyInstance();
}
