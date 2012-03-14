/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/widgets/talkable-menu-manager.h"
#include "protocols/services/chat-service.h"

#include "keys/keys-manager.h"
#include "notify/encryption-ng-notification.h"

#include "send-public-key-action-description.h"

SendPublicKeyActionDescription::SendPublicKeyActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeUser);
	setName("sendPublicKeyAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Send My Public Key"));

	registerAction();

	TalkableMenuManager::instance()->addListActionDescription(this,
			TalkableMenuItem::CategoryManagement, 20);
}

SendPublicKeyActionDescription::~SendPublicKeyActionDescription()
{
	TalkableMenuManager::instance()->removeListActionDescription(this);
}

void SendPublicKeyActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	foreach (const Contact &contact, action->context()->contacts())
		sendPublicKey(contact);
}

void SendPublicKeyActionDescription::updateActionState(Action *action)
{
	action->setEnabled(false);

	ContactSet contacts = action->context()->contacts();
	if (contacts.isEmpty())
		return;

	if (action->context()->buddies().contains(Core::instance()->myself()))
		return;

	foreach (const Contact &contact, contacts)
	{
		Contact accountContact = contact.contactAccount().accountContact();
		// TODO: this should depend on submodule and not be hardcoded
		// TODO 0.10:
		Key key = KeysManager::instance()->byContactAndType(accountContact, "simlite", ActionReturnNull);
		if (key)
		{
			action->setEnabled(true);
			return;
		}
	}
}

void SendPublicKeyActionDescription::sendPublicKey(const Contact &contact)
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

	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	chatService->sendMessage(chat, QString::fromUtf8(key.key().data()), true);

	EncryptionNgNotification::notifyPublicKeySent(contact);
}
