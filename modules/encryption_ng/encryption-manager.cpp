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

#include "gui/widgets/chat-widget-manager.h"
#include "protocols/services/chat-service.h"
#include "protocols/protocol.h"

#include "configuration/encryption-ng-configuration.h"
#include "decryptor.h"
#include "encryption-actions.h"
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

EncryptionManager::EncryptionManager() :
		Generator(0)
{
	triggerAllAccountsRegistered();

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)),
			this, SLOT(chatWidgetDestroying(ChatWidget*)));
}

EncryptionManager::~EncryptionManager()
{
	triggerAllAccountsUnregistered();

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)),
			this, SLOT(chatWidgetCreated(ChatWidget*)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)),
			this, SLOT(chatWidgetDestroying(ChatWidget*)));
}

void EncryptionManager::accountRegistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
				this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
		connect(chatService, SIGNAL(filterRawOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterRawOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

void EncryptionManager::accountUnregistered(Account account)
{
	if (!account.protocolHandler())
		return;

	ChatService *chatService = account.protocolHandler()->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)),
				this, SLOT(filterRawIncomingMessage(Chat,Contact,QByteArray&,bool&)));
		disconnect(chatService, SIGNAL(filterRawOutgoingMessage(Chat,QByteArray&,bool&)),
				this, SLOT(filterRawOutgoingMessage(Chat,QByteArray&,bool&)));
	}
}

bool EncryptionManager::setEncryptionEnabled(const Chat &chat, bool enable)
{
	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", true);
	if (enable)
	{
		// just in case release previous one
		Encryptor *encryptor = encryptionChatData->encryptor();
		if (encryptor)
			encryptor->provider()->releaseEncryptor(chat, encryptor);

		encryptor = EncryptionProviderManager::instance()->acquireEncryptor(chat);
		encryptionChatData->setEncryptor(encryptor);

		EncryptionActions::instance()->checkEnableEncryption(chat, 0 != encryptor);
		encryptionChatData->setEncrypt(0 != encryptor);
		return 0 != encryptor;
	}
	else
	{
		Encryptor *encryptor = encryptionChatData->encryptor();
		if (encryptor)
			encryptor->provider()->releaseEncryptor(chat, encryptor);
		encryptionChatData->setEncryptor(0);
		encryptionChatData->setEncrypt(false);

		EncryptionActions::instance()->checkEnableEncryption(chat, false);
		return true; // we can always disable
	}
}

void EncryptionManager::filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore)
{
	Q_UNUSED(sender)
	Q_UNUSED(ignore)

	if (!chat)
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", true);
	if (!encryptionChatData)
		return;

	if (!encryptionChatData->decryptor())
		encryptionChatData->setDecryptor(EncryptionProviderManager::instance()->acquireDecryptor(chat));

	bool decrypted;
	message = encryptionChatData->decryptor()->decrypt(message, &decrypted);

	if (decrypted && EncryptionNgConfiguration::instance()->encryptAfterReceiveEncryptedMessage())
		setEncryptionEnabled(chat, true);
}

void EncryptionManager::filterRawOutgoingMessage(Chat chat, QByteArray &message, bool &stop)
{
	Q_UNUSED(stop)

	if (!chat)
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng");
	if (encryptionChatData && encryptionChatData->encryptor())
		message = encryptionChatData->encryptor()->encrypt(message);
}

void EncryptionManager::chatWidgetCreated(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng", true);
	if (encryptionChatData->encrypt())
		setEncryptionEnabled(chat, true);
}

void EncryptionManager::chatWidgetDestroying(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	EncryptionChatData *encryptionChatData = chat.data()->moduleStorableData<EncryptionChatData>("encryption-ng");

	// free some memory, these objects will be recreated when needed
	if (encryptionChatData->decryptor())
	{
		encryptionChatData->decryptor()->provider()->releaseDecryptor(chat, encryptionChatData->decryptor());
		encryptionChatData->setDecryptor(0);
	}
	if (encryptionChatData->encryptor())
	{
		encryptionChatData->encryptor()->provider()->releaseEncryptor(chat, encryptionChatData->encryptor());
		encryptionChatData->setEncryptor(0);
	}
}

void EncryptionManager::setGenerator(KeyGenerator *generator)
{
	Generator = generator;
}

KeyGenerator * EncryptionManager::generator()
{
	return Generator;
}
