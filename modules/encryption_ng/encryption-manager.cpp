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

#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "encryption-chat-data.h"
#include "encryption-provider-manager.h"
#include "encryptor.h"

#include "encryption-manager.h"

EncryptionManager * EncryptionManager::Instance = 0;

void EncryptionManager::createInstance()
{
	Instance = new EncryptionManager();
}

void EncryptionManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

EncryptionManager::EncryptionManager()
{
	triggerAllAccountsRegistered();
}

EncryptionManager::~EncryptionManager()
{
	triggerAllAccountsUnregistered();
}

void EncryptionManager::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterIncomingMessage(Chat,Contact,QString,time_t,bool&)),
				this, SLOT(filterIncomingMessage(Chat,Contact,QString,time_t,bool&)));
		connect(chatService, SIGNAL(filterOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

void EncryptionManager::accountUnregistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(filterIncomingMessage(Chat,Contact,QString,time_t,bool&)),
				this, SLOT(filterIncomingMessage(Chat,Contact,QString,time_t,bool&)));
		disconnect(chatService, SIGNAL(filterOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

void EncryptionManager::setEnableEncryption(const Chat &chat, bool enable)
{
	// TODO: find out life-cycle of encryptor-decryptor classes
	EncryptionChatData *encryptionChatData = chat.data()->moduleData<EncryptionChatData>("encryption-ng", true);
	if (enable)
	{
		Encryptor *encryptor = EncryptionProviderManager::instance()->encryptor(chat);
		encryptionChatData->setEncryptor(encryptor);
	}
	else
		// destroy it here or sth?
		encryptionChatData->setEncryptor(0);
}

void EncryptionManager::filterIncomingMessage(Chat chat, Contact sender, const QString &message, time_t time, bool &ignore)
{
	Q_UNUSED(chat)
	Q_UNUSED(sender)
	Q_UNUSED(message)
	Q_UNUSED(time)
	Q_UNUSED(ignore)
}

void EncryptionManager::filterOutgoingMessage(Chat chat, QByteArray &message, bool &stop)
{
	Q_UNUSED(stop)

	EncryptionChatData *encryptionChatData = chat.data()->moduleData<EncryptionChatData>("encryption-ng");
	if (encryptionChatData && encryptionChatData->encryptor())
		message = encryptionChatData->encryptor()->encrypt(message);
}
