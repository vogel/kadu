/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/path-conversion.h"

#include "encryption-ng-configuration-ui-handler.h"

EncryptionNgConfigurationUiHandler * EncryptionNgConfigurationUiHandler::Instance = 0;

void EncryptionNgConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new EncryptionNgConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/encryption-ng.ui"));
	}
}

void EncryptionNgConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/encryption-ng.ui"));
		delete Instance;
		Instance = 0;
	}
}

EncryptionNgConfigurationUiHandler::EncryptionNgConfigurationUiHandler()
{
}

EncryptionNgConfigurationUiHandler::~EncryptionNgConfigurationUiHandler()
{
}
