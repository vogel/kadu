/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCrypto>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-action-button.h"
#include "misc/path-conversion.h"
#include "protocols/protocol.h"

#include "encryption-manager.h"
#include "encryption-ng-configuration-ui-handler.h"
#include "keys/keys-manager.h"
#include "key-generator.h"
#include <gui/windows/message-dialog.h>

EncryptionNgConfigurationUiHandler * EncryptionNgConfigurationUiHandler::Instance = 0;

void EncryptionNgConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new EncryptionNgConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/encryption-ng.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void EncryptionNgConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/encryption-ng.ui"));
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

void EncryptionNgConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	GenerateKeys = static_cast<ConfigActionButton *>(mainConfigurationWindow->widget()->widgetById("encryption-ng/generateKeys"));
	connect(GenerateKeys, SIGNAL(clicked()), this, SLOT(generateKeysClicked()));
}

void EncryptionNgConfigurationUiHandler::generateKeysClicked()
{
	QMenu menu;

	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolHandler())
		{
			QString title = QString("%1 (%2)")
					.arg(account.accountIdentity().name())
					.arg(account.id());
			QAction *action = menu.addAction(account.protocolHandler()->icon(), title, this, SLOT(generateKeys()));
			action->setData(account);
		}

	menu.exec(GenerateKeys->mapToGlobal(QPoint(0, GenerateKeys->height())));
}

void EncryptionNgConfigurationUiHandler::generateKeys()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;

	Account account = action->data().value<Account>();
	if (!account)
		return;

	KeyGenerator *generator = EncryptionManager::instance()->generator();
	if (!generator)
	{
		MessageDialog::exec("dialog-error", tr("Encryption"), tr("Cannot generate keys. Check if encryption_simlite module is loaded"));
		return;
	}

	if (generator->hasKeys(account))
		if (!MessageDialog::ask("dialog-information", tr("Encryption"), tr("Keys exist. Do you want to overwrite them?")))
			return;

	if (generator->generateKeys(account))
		MessageDialog::exec("dialog-information", tr("Encryption"), tr("Keys have been generated"));
	else
		MessageDialog::exec("dialog-error", tr("Encryption"), tr("Error generating keys"));
}
