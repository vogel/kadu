/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "services/raw-message-transformer-service.h"
#include <gui/widgets/account-configuration-widget-factory-repository.h>

#include "gui/widgets/encryption-ng-otr-account-configuration-widget-factory.h"
#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-private-key-service.h"
#include "encryption-ng-otr-raw-message-transformer.h"
#include "encryption-ng-otr-user-state.h"

#include "encryption-ng-otr-plugin.h"

EncryptionNgOtrPlugin::EncryptionNgOtrPlugin()
{
	OTRL_INIT;
}

EncryptionNgOtrPlugin::~EncryptionNgOtrPlugin()
{
}

void EncryptionNgOtrPlugin::registerOtrAcountConfigurationWidgetFactory()
{
	OtrAccountConfigurationWidgetFactory.reset(new EncryptionNgOtrAccountConfigurationWidgetFactory());

	Core::instance()->accountConfigurationWidgetFactoryRepository()->registerFactory(OtrAccountConfigurationWidgetFactory.data());
}

void EncryptionNgOtrPlugin::unregisterOtrAcountConfigurationWidgetFactory()
{
	Core::instance()->accountConfigurationWidgetFactoryRepository()->unregisterFactory(OtrAccountConfigurationWidgetFactory.data());

	OtrAccountConfigurationWidgetFactory.reset();
}

void EncryptionNgOtrPlugin::registerOtrAppOpsWrapper()
{
	OtrAppOpsWrapper.reset(new EncryptionNgOtrAppOpsWrapper());
}

void EncryptionNgOtrPlugin::unregisterOtrAppOpsWrapper()
{
	OtrAppOpsWrapper.reset();
}

void EncryptionNgOtrPlugin::registerOtrPrivateKeyService()
{
	OtrPrivateKeyService.reset(new EncryptionNgOtrPrivateKeyService());
}

void EncryptionNgOtrPlugin::unregisterOtrPrivateKeyService()
{
	OtrPrivateKeyService.reset();
}

void EncryptionNgOtrPlugin::registerOtrRawMessageTransformer()
{
	OtrRawMessageTransformer.reset(new EncryptionNgOtrRawMessageTransformer());

	Core::instance()->rawMessageTransformerService()->registerTransformer(OtrRawMessageTransformer.data());
}

void EncryptionNgOtrPlugin::unregisterOtrRawMessageTransformer()
{
	Core::instance()->rawMessageTransformerService()->unregisterTransformer(OtrRawMessageTransformer.data());

	OtrRawMessageTransformer.reset();
}

int EncryptionNgOtrPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad);

	registerOtrAcountConfigurationWidgetFactory();
	registerOtrAppOpsWrapper();
	registerOtrPrivateKeyService();
	registerOtrRawMessageTransformer();

	OtrPrivateKeyService->setUserState(&OtrUserState);
	OtrPrivateKeyService->readPrivateKeys();

	OtrRawMessageTransformer->setEncryptionNgOtrAppOpsWrapper(OtrAppOpsWrapper.data());
	OtrRawMessageTransformer->setEncryptionNgOtrPrivateKeyService(OtrPrivateKeyService.data());
	OtrRawMessageTransformer->setUserState(&OtrUserState);

	return 0;
}

void EncryptionNgOtrPlugin::done()
{
	OtrRawMessageTransformer->setUserState(0);
	OtrRawMessageTransformer->setEncryptionNgOtrPrivateKeyService(0);
	OtrRawMessageTransformer->setEncryptionNgOtrAppOpsWrapper(0);

	OtrPrivateKeyService->setUserState(0);

	unregisterOtrRawMessageTransformer();
	unregisterOtrPrivateKeyService();
	unregisterOtrAppOpsWrapper();
	unregisterOtrAcountConfigurationWidgetFactory();
}

Q_EXPORT_PLUGIN2(encryption_ng_otr, EncryptionNgOtrPlugin)
