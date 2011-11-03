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

#include "accounts/account-manager.h"
#include "chat/chat.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "identities/identity.h"
#include "protocols/protocol.h"

#include "actions/enable-encryption-action-description.h"
#include "actions/send-public-key-action-description.h"
#include "keys/key.h"
#include "encryption-manager.h"
#include "key-generator.h"

#include "encryption-actions.h"

EncryptionActions * EncryptionActions::Instance = 0;

void EncryptionActions::registerActions()
{
	if (!Instance)
		Instance = new EncryptionActions();
}

void EncryptionActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

EncryptionActions::EncryptionActions()
{
	GenerateKeysMenu = new QMenu(0);
	connect(GenerateKeysMenu, SIGNAL(triggered(QAction*)), this, SLOT(generateKeysActionActivated(QAction*)));

	GenerateKeysActionDescription = new ActionDescription(this,
			ActionDescription::TypeMainMenu, "encryptionGenerateKeysAction",
			this, 0, KaduIcon("security-high"), tr("Generate Encryption Keys")
	);
	connect(GenerateKeysActionDescription, SIGNAL(actionCreated(Action*)), this, SLOT(generateKeysActionCreated(Action*)));
	Core::instance()->kaduWindow()->insertMenuActionDescription(GenerateKeysActionDescription, KaduWindow::MenuTools);

	updateGenerateKeysMenu();

	EnableEncryptionActionDescriptionInstance = new EnableEncryptionActionDescription(this);
	new SendPublicKeyActionDescription(this);
}

EncryptionActions::~EncryptionActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(GenerateKeysActionDescription);

	// actions is owner of menu, no need to delete here
	GenerateKeysMenu = 0;
}

void EncryptionActions::generateKeysActionCreated(Action *action)
{
	action->setMenu(GenerateKeysMenu);
	action->setEnabled(!GenerateKeysMenu->isEmpty());
}

void EncryptionActions::generateKeysActionActivated(QAction *action)
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

void EncryptionActions::accountRegistered(Account account)
{
	Q_UNUSED(account)

	updateGenerateKeysMenu();
}

void EncryptionActions::accountUnregistered(Account account)
{
	Q_UNUSED(account)

	updateGenerateKeysMenu();
}

void EncryptionActions::updateGenerateKeysMenu()
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
	foreach (Action *action, GenerateKeysActionDescription->actions())
		action->setEnabled(enable);
}

void EncryptionActions::checkEnableEncryption(const Chat &chat, bool check)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, EnableEncryptionActionDescriptionInstance->actions())
		if (action->chat() == chat)
			action->setChecked(check);
}
