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
#include "chat/chat-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "keys/key.h"
#include "keys/keys-manager.h"
#include "notify/encryption-ng-notification.h"
#include "encryption-manager.h"
#include "encryption-provider-manager.h"
#include "key-generator.h"

#include "encryption-actions.h"

static void checkCanEncrypt(Action *action)
{
	Chat chat = action->chat();
	if (!chat)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(EncryptionProviderManager::instance()->canEncrypt(chat));
}

static void checkSendKey(Action *action)
{
	action->setEnabled(false);

	ContactSet contacts = action->contacts();
	if (contacts.isEmpty())
		return;

	foreach (const Contact &contact, contacts)
	{
		Contact accountContact = contact.contactAccount().accountContact();
		// TODO: this should depend on submodule and not be hardcoded
		// TODO: 0.8
		Key key = KeysManager::instance()->byContactAndType(accountContact, "simlite", ActionReturnNull);
		if (key)
		{
			action->setEnabled(true);
			return;
		}
	}
}

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
	GenerateKeysActionDescription = new ActionDescription(this,
			ActionDescription::TypeMainMenu, "encryptionGenerateKeysAction",
			this, 0, "security-high", tr("Generate Encryption Keys")
	);
	connect(GenerateKeysActionDescription, SIGNAL(actionCreated(Action*)), this, SLOT(generateKeysActionCreated(Action*)));

	// HACK: It is needed bacause of loading protocol modules before creating GUI.
	// TODO 0.8: Fix it!
	QMetaObject::invokeMethod(this, "insertMenuToMainWindow", Qt::QueuedConnection);

	GenerateKeysMenu = new QMenu(0);
	connect(GenerateKeysMenu, SIGNAL(triggered(QAction*)), this, SLOT(generateKeysActionActivated(QAction*)));

	updateGenerateKeysMenu();

	EnableEncryptionActionDescription = new ActionDescription(this,
			ActionDescription::TypeChat, "encryptionAction",
			this, SLOT(enableEncryptionActionActivated(QAction *, bool)),
			"security-high", tr("Encrypt"),
			true, checkCanEncrypt
	);

	SendPublicKeyActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "sendPublicKeyAction",
		this, SLOT(sendPublicKeyActionActivated(QAction *, bool)),
		"security-high", tr("Send My Public Key"),
		false, checkSendKey
	);
	BuddiesListViewMenuManager::instance()->addListActionDescription(SendPublicKeyActionDescription,
			BuddiesListViewMenuItem::MenuCategoryManagement, 20);;

	connect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)), this, SLOT(canEncryptChanged(Chat)));
}

EncryptionActions::~EncryptionActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(GenerateKeysActionDescription);

	disconnect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)), this, SLOT(canEncryptChanged(Chat)));

	delete GenerateKeysMenu;
	GenerateKeysMenu = 0;
}

void EncryptionActions::insertMenuToMainWindow()
{
	Core::instance()->kaduWindow()->insertMenuActionDescription(GenerateKeysActionDescription, KaduWindow::MenuTools);
}

void EncryptionActions::canEncryptChanged(const Chat &chat)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, EnableEncryptionActionDescription->actions())
		if (action->chat() == chat)
			action->checkState();
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

void EncryptionActions::enableEncryptionActionActivated(QAction *sender, bool toggled)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	if (!action->chat())
		return;

	if (!EncryptionManager::instance()->setEncryptionEnabled(action->chat(), toggled))
	{
		// disable it, we could not enable encryption for this contact
		sender->setEnabled(false);
		sender->setChecked(false);
	}
}

void EncryptionActions::sendPublicKeyActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	foreach (const Contact &contact, action->contacts())
		sendPublicKey(contact);
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

void EncryptionActions::sendPublicKey(const Contact &contact)
{
	Account account = contact.contactAccount();
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	Key key = KeysManager::instance()->byContactAndType(account.accountContact(), "simlite", ActionReturnNull);
	if (!key)
	{
		EncryptionNgNotification::notifyPublicKeySendError(contact, tr("No public key available"));
		return;
	}

	ContactSet contacts;
	contacts.insert(contact);

	Chat chat = ChatManager::instance()->findChat(contacts, true);
	chatService->sendMessage(chat, QString::fromUtf8(key.key().data()), true);

	EncryptionNgNotification::notifyPublicKeySent(contact);
}

void EncryptionActions::checkEnableEncryption(const Chat &chat, bool check)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, EnableEncryptionActionDescription->actions())
		if (action->chat() == chat)
			action->setChecked(check);
}
