/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "identities/identity.h"

#include "keys/key.h"
#include "encryption-manager.h"
#include "key-generator.h"

#include "generate-keys-action-description.h"

GenerateKeysActionDescription::GenerateKeysActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("encryptionGenerateKeysAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Generate Encryption Keys"));

	GenerateKeysMenu = new QMenu();
	connect(GenerateKeysMenu, SIGNAL(triggered(QAction*)),
			this, SLOT(menuActionTriggered(QAction*)));
	updateGenerateKeysMenu();

	registerAction();

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
			this, SLOT(updateGenerateKeysMenu()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
			this, SLOT(updateGenerateKeysMenu()));

	Core::instance()->kaduWindow()->insertMenuActionDescription(this, KaduWindow::MenuTools);
}

GenerateKeysActionDescription::~GenerateKeysActionDescription()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(this);

	// actions is owner of menu, no need to delete here
	GenerateKeysMenu = 0;
}

void GenerateKeysActionDescription::actionInstanceCreated(Action *action)
{
	action->setMenu(GenerateKeysMenu);
	action->setEnabled(!GenerateKeysMenu->isEmpty());
}

void GenerateKeysActionDescription::updateGenerateKeysMenu()
{
	GenerateKeysMenu->clear();

	foreach (const Account &account, AccountManager::instance()->items())
		if (account.data() && account.details())
		{
			QAction *action = new QAction(QString("%1 (%2)").arg(account.accountIdentity().name()).arg(account.id()), GenerateKeysMenu);
			action->setData(QVariant::fromValue(account));
			GenerateKeysMenu->addAction(action);
		}

	bool enable = !GenerateKeysMenu->actions().isEmpty();
	foreach (Action *action, actions())
		action->setEnabled(enable);
}

void GenerateKeysActionDescription::menuActionTriggered(QAction *action)
{
	Account account = action->data().value<Account>();
	if (!account)
		return;

	KeyGenerator *generator = EncryptionManager::instance()->generator();
	if (!generator)
	{
		MessageDialog::exec(KaduIcon("dialog-error"), tr("Encryption"), tr("Cannot generate keys. Check if encryption_simlite module is loaded"));
		return;
	}

	if (generator->hasKeys(account))
		if (!MessageDialog::ask(KaduIcon("dialog-information"), tr("Encryption"), tr("Keys exist. Do you want to overwrite them?")))
			return;

	if (generator->generateKeys(account))
		MessageDialog::exec(KaduIcon("dialog-information"), tr("Encryption"), tr("Keys have been generated"));
	else
		MessageDialog::exec(KaduIcon("dialog-error"), tr("Encryption"), tr("Error generating keys"));
}
