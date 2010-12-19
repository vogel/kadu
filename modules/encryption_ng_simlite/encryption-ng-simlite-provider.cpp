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

#include "contacts/contact-set.h"
#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "modules/encryption_ng/keys/keys-manager.h"

#include "encryption-ng-simlite-encryptor.h"

#include "encryption-ng-simlite-provider.h"

#define RSA_PUBLIC_KEY_BEGIN "-----BEGIN RSA PUBLIC KEY-----"

EncryptioNgSimliteProvider * EncryptioNgSimliteProvider::Instance = 0;

void EncryptioNgSimliteProvider::createInstance()
{
	Instance = new EncryptioNgSimliteProvider();
}

void EncryptioNgSimliteProvider::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptioNgSimliteProvider::EncryptioNgSimliteProvider()
{
	triggerAllAccountsRegistered();
}

EncryptioNgSimliteProvider::~EncryptioNgSimliteProvider()
{
	triggerAllAccountsUnregistered();
}

void EncryptioNgSimliteProvider::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	connect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
			this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
}

void EncryptioNgSimliteProvider::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	connect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
			this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
}

void EncryptioNgSimliteProvider::filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore)
{
	Q_UNUSED(chat)
	if (!message.startsWith(RSA_PUBLIC_KEY_BEGIN))
		return;

	emit keyReceived(sender, "simlite", message);
	ignore = true;
}

bool EncryptioNgSimliteProvider::canDecrypt(const Chat &chat)
{
	Q_UNUSED(chat)
	return false;
}

bool EncryptioNgSimliteProvider::canEncrypt(const Chat &chat)
{
	if (1 != chat.contacts().size())
		return false;

	Key key = KeysManager::instance()->byContactAndType(*chat.contacts().begin(), "simlite", ActionReturnNull);
	return !key.isNull();
}

Decryptor * EncryptioNgSimliteProvider::decryptor(const Chat &chat)
{
	Q_UNUSED(chat)
	return 0;
}

Encryptor * EncryptioNgSimliteProvider::encryptor(const Chat &chat)
{
	if (1 != chat.contacts().size())
		return 0;

	Key key = KeysManager::instance()->byContactAndType(*chat.contacts().begin(), "simlite", ActionReturnNull);
	if (!key)
		return 0;

	return new EncryptioNgSimliteEncryptor(key);
}
