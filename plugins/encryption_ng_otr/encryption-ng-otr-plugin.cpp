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

#include "encryption-ng-otr-app-ops-wrapper.h"
#include "encryption-ng-otr-private-key-service.h"
#include "encryption-ng-otr-raw-message-transformer.h"
#include "encryption-ng-otr-user-state-service.h"

#include "encryption-ng-otr-plugin.h"

EncryptionNgOtrPlugin::EncryptionNgOtrPlugin()
{
	OTRL_INIT;
}

EncryptionNgOtrPlugin::~EncryptionNgOtrPlugin()
{
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

void EncryptionNgOtrPlugin::registerOtrUserStateService()
{
	OtrUserStateService.reset(new EncryptionNgOtrUserStateService(this));
}

void EncryptionNgOtrPlugin::unregisterOtrUserStateService()
{
	OtrUserStateService.reset();
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

	registerOtrAppOpsWrapper();
	registerOtrPrivateKeyService();
	registerOtrUserStateService();
	registerOtrRawMessageTransformer();

	OtrPrivateKeyService->setEncryptionNgOtrUserStateService(OtrUserStateService.data());

	OtrRawMessageTransformer->setEncryptionNgOtrAppOpsWrapper(OtrAppOpsWrapper.data());
	OtrRawMessageTransformer->setEncryptionNgOtrPrivateKeyService(OtrPrivateKeyService.data());
	OtrRawMessageTransformer->setEncryptionNgOtrUserStateService(OtrUserStateService.data());

	return 0;
}

void EncryptionNgOtrPlugin::done()
{
	OtrRawMessageTransformer->setEncryptionNgOtrUserStateService(0);
	OtrRawMessageTransformer->setEncryptionNgOtrPrivateKeyService(0);
	OtrRawMessageTransformer->setEncryptionNgOtrAppOpsWrapper(0);

	OtrPrivateKeyService->setEncryptionNgOtrUserStateService(0);

	unregisterOtrRawMessageTransformer();
	unregisterOtrUserStateService();
	unregisterOtrPrivateKeyService();
	unregisterOtrAppOpsWrapper();
}

Q_EXPORT_PLUGIN2(encryption_ng_otr, EncryptionNgOtrPlugin)
