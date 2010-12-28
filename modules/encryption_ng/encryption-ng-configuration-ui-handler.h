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

#ifndef ENCRYPTION_NG_CONFIGURATION_UI_HANDLER
#define ENCRYPTION_NG_CONFIGURATION_UI_HANDLER

#include "gui/windows/main-configuration-window.h"

class Account;
class ConfigActionButton;

class EncryptionNgConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	static EncryptionNgConfigurationUiHandler * Instance;

	ConfigActionButton *GenerateKeys;

	explicit EncryptionNgConfigurationUiHandler();
	virtual ~EncryptionNgConfigurationUiHandler();

	bool generateKeys(const Account &account);

private slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void generateKeysClicked();
	void generateKeys();

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

};

#endif // ENCRYPTION_NG_CONFIGURATION_UI_HANDLER
