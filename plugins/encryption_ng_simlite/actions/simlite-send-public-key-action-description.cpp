/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"

#include "plugins/encryption_ng/encryption-manager.h"
#include "plugins/encryption_ng/key-generator.h"
#include "plugins/encryption_ng/keys/keys-manager.h"
#include "plugins/encryption_ng/notify/encryption-ng-notification.h"

#include "simlite-send-public-key-action-description.h"

SimliteSendPublicKeyActionDescription::SimliteSendPublicKeyActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypePrivate);
	setName("simliteSendPublicKeyAction");
	setText(tr("Send My Public Key (Simlite)"));

	registerAction();

	MenuInventory::instance()
		->menu("buddy-list")
		->addAction(this, KaduMenu::SectionActions, 200)
		->update();
}

SimliteSendPublicKeyActionDescription::~SimliteSendPublicKeyActionDescription()
{
	MenuInventory::instance()
		->menu("buddy-list")
		->removeAction(this)
		->update();
}

void SimliteSendPublicKeyActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	foreach (const Contact &contact, action->context()->contacts())
		sendPublicKey(contact);
}

void SimliteSendPublicKeyActionDescription::sendPublicKey(const Contact &contact)
{
	KeyGenerator *generator = EncryptionManager::instance()->generator();
	if (!generator)
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Encryption"), tr("Cannot send keys. Check if encryption_ng_simlite plugin is loaded"));
		return;
	}

	if (!generator->hasKeys(contact.contactAccount()))
	{
		MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-information"), tr("Encryption"), tr("Public key dont exist. Do you want to create new one?"));
		dialog->addButton(QMessageBox::Yes, tr("Yes"));
		dialog->addButton(QMessageBox::No, tr("No"));

		if (!dialog->ask())
			return;

		if (!generator->generateKeys(contact.contactAccount()))
		{
			MessageDialog::show(KaduIcon("dialog-error"), tr("Encryption"), tr("Error generating key"));
			return;
		}
	}

	Key key = KeysManager::instance()->byContactAndType(contact.contactAccount().accountContact(), "simlite", ActionReturnNull);
	if (!key)
	{
		EncryptionNgNotification::notifyPublicKeySendError(contact, tr("No public key available"));
		return;
	}

	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	if (MessageManager::instance()->sendRawMessage(chat, key.key().data()))
		EncryptionNgNotification::notifyPublicKeySent(contact);
}

#include "moc_simlite-send-public-key-action-description.cpp"
