/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "message/raw-message.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"
#include "services/raw-message-transformer-service.h"

#include "decryptor.h"
#include "encryption-actions.h"
#include "encryption-chat-data.h"
#include "encryption-provider-manager.h"
#include "encryptor.h"

#include "encryption-manager.h"

EncryptionManager * EncryptionManager::m_instance = 0;

void EncryptionManager::createInstance()
{
	if (!m_instance)
	{
		auto manager = new EncryptionManager();
		manager->setChatWidgetRepository(Core::instance()->chatWidgetRepository());
	}
}

void EncryptionManager::destroyInstance()
{
	delete m_instance;
}

EncryptionManager::EncryptionManager() :
		m_generator(0)
{
	m_instance = this;

	Core::instance()->rawMessageTransformerService()->registerTransformer(this);
}

EncryptionManager::~EncryptionManager()
{
	Core::instance()->rawMessageTransformerService()->unregisterTransformer(this);

	if (m_chatWidgetRepository)
	{
		disconnect(m_chatWidgetRepository.data(), 0, this, 0);

		for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
			chatWidgetRemoved(chatWidget);
	}

	m_instance = 0;
}

void EncryptionManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (!m_chatWidgetRepository)
		return;

	for (ChatWidget *chatWidget : m_chatWidgetRepository.data())
		chatWidgetAdded(chatWidget);

	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget*)),
			this, SLOT(chatWidgetAdded(ChatWidget*)));
	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)),
			this, SLOT(chatWidgetRemoved(ChatWidget*)));
}

EncryptionChatData * EncryptionManager::chatEncryption(const Chat &chat)
{
	if (!m_chatEnryptions.contains(chat))
		m_chatEnryptions.insert(chat, new EncryptionChatData(chat, this));

	return m_chatEnryptions.value(chat);
}

void EncryptionManager::setEncryptionProvider(const Chat &chat, EncryptionProvider *encryptionProvider)
{
	if (!chat)
		return;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);
	Encryptor *currentEncryptor = encryptionChatData->encryptor();

	if (currentEncryptor && currentEncryptor->provider() == encryptionProvider)
		return;

	if (currentEncryptor)
		currentEncryptor->provider()->releaseEncryptor(chat, currentEncryptor);

	encryptionChatData->setEncryptor(encryptionProvider ? encryptionProvider->acquireEncryptor(chat) : 0);
	EncryptionActions::instance()->checkEnableEncryption(chat, encryptionChatData->encryptor());
}

EncryptionProvider * EncryptionManager::encryptionProvider(const Chat &chat)
{
	if (!chat)
		return 0;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);
	Encryptor *currentEncryptor = encryptionChatData->encryptor();
	if (!currentEncryptor)
		return 0;

	return currentEncryptor->provider();
}

void EncryptionManager::chatWidgetAdded(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	if (!EncryptionProviderManager::instance()->canEncrypt(chat))
		return;

	if (chatEncryption(chat)->encrypt())
	{
		EncryptionProvider *encryptorProvider = EncryptionProviderManager::instance()->defaultEncryptorProvider(chat);
		EncryptionManager::instance()->setEncryptionProvider(chat, encryptorProvider);
	}
}

void EncryptionManager::chatWidgetRemoved(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (!chat.data())
		return;

	EncryptionChatData *encryptionChatData = chatEncryption(chat);

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
	m_generator = generator;
}

KeyGenerator * EncryptionManager::generator()
{
	return m_generator;
}

RawMessage EncryptionManager::transformIncomingMessage(const RawMessage &rawMessage, const Message &message)
{
	if (!message.messageChat())
		return rawMessage;

	if (!EncryptionProviderManager::instance()->canDecrypt(message.messageChat()))
		return rawMessage;

	EncryptionChatData *encryptionChatData = chatEncryption(message.messageChat());
	if (!encryptionChatData->decryptor())
		encryptionChatData->setDecryptor(EncryptionProviderManager::instance()->acquireDecryptor(message.messageChat()));

	bool decrypted;
	return encryptionChatData->decryptor()->decrypt(rawMessage, message.messageChat(), &decrypted);
}

RawMessage EncryptionManager::transformOutgoingMessage(const RawMessage &rawMessage, const Message &message)
{
	if (!message.messageChat())
		return rawMessage;

	EncryptionChatData *encryptionChatData = chatEncryption(message.messageChat());
	if (encryptionChatData && encryptionChatData->encryptor())
		return encryptionChatData->encryptor()->encrypt(rawMessage);

	return rawMessage;
}

RawMessage EncryptionManager::transform(const RawMessage &rawMessage, const Message& message)
{
	switch (message.type())
	{
		case MessageTypeSent:
			return transformOutgoingMessage(rawMessage, message);
		case MessageTypeReceived:
			return transformIncomingMessage(rawMessage, message);
		default:
			return rawMessage;
	}
}

#include "moc_encryption-manager.cpp"
