/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/application.h"
#include "core/core.h"
#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "plugins/encryption_ng/encryption-manager.h"
#include "plugins/encryption_ng/encryption-provider-manager.h"
#include "services/message-filter-service.h"
#include "exports.h"

#include "actions/simlite-send-public-key-action-description.h"
#include "configuration/encryption-ng-simlite-configuration.h"
#include "encryption-ng-simlite-key-generator.h"
#include "encryption-ng-simlite-key-importer.h"
#include "encryption-ng-simlite-message-filter.h"
#include "encryption-ng-simlite-provider.h"

#include "encryption-ng-simlite-plugin.h"

EngryptionNgSimlitePlugin::~EngryptionNgSimlitePlugin()
{
}

bool EngryptionNgSimlitePlugin::init(bool firstLoad)
{
	if (firstLoad)
		EncryptioNgSimliteKeyImporter::createInstance();

	EncryptionNgSimliteConfiguration::createInstance();

	EncryptioNgSimliteKeyGenerator::createInstance();
	EncryptionManager::instance()->setGenerator(EncryptioNgSimliteKeyGenerator::instance());

	EncryptioNgSimliteProvider::createInstance();
	MessageFilter = new EncryptionNgSimliteMessageFilter(this);
	Core::instance()->messageFilterService()->registerMessageFilter(MessageFilter);
	EncryptioNgSimliteProvider::instance()->setMessageFilter(MessageFilter);

	EncryptionProviderManager::instance()->registerProvider(EncryptioNgSimliteProvider::instance());

	new SimliteSendPublicKeyActionDescription(this);
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/encryption-ng-simlite.ui"));
	MenuInventory::instance()->menu("encryption-ng")->addAction(Actions::instance()->value("simliteSendPublicKeyAction"), KaduMenu::SectionConfig);
	MenuInventory::instance()->menu("encryption-ng")->update();

	return true;
}

void EngryptionNgSimlitePlugin::done()
{
	MenuInventory::instance()->menu("encryption-ng")->removeAction(Actions::instance()->value("simliteSendPublicKeyAction"));
	MenuInventory::instance()->menu("encryption-ng")->update();
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/encryption-ng-simlite.ui"));

	Core::instance()->messageFilterService()->unregisterMessageFilter(MessageFilter);

	EncryptionProviderManager::instance()->unregisterProvider(EncryptioNgSimliteProvider::instance());
	EncryptioNgSimliteProvider::destroyInstance();

	EncryptionManager::instance()->setGenerator(0);
	EncryptioNgSimliteKeyGenerator::destroyInstance();

	EncryptionNgSimliteConfiguration::destroyInstance();

	// it can work without createInstance too, so don't care about firstLoad here
	EncryptioNgSimliteKeyImporter::destroyInstance();
}

Q_EXPORT_PLUGIN2(encryption_ng_simlite, EngryptionNgSimlitePlugin)

#include "moc_encryption-ng-simlite-plugin.cpp"
