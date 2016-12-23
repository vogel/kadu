/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message-manager-impl.h"

#include "accounts/account-manager.h"
#include "message/message-filter-service.h"
#include "message/message-storage.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"
#include "services/message-transformer-service.h"

MessageManagerImpl::MessageManagerImpl(QObject *parent) :
		MessageManager{parent}
{
}

MessageManagerImpl::~MessageManagerImpl()
{
}

void MessageManagerImpl::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void MessageManagerImpl::setMessageFilterService(MessageFilterService *messageFilterService)
{
	m_messageFilterService = messageFilterService;
}

void MessageManagerImpl::setMessageStorage(MessageStorage *messageStorage)
{
	m_messageStorage = messageStorage;
}

void MessageManagerImpl::setMessageTransformerService(MessageTransformerService *messageTransformerService)
{
	m_messageTransformerService = messageTransformerService;
}

void MessageManagerImpl::init()
{
	triggerAllAccountsAdded(m_accountManager);
}

void MessageManagerImpl::done()
{
	triggerAllAccountsRemoved(m_accountManager);
}

void MessageManagerImpl::accountAdded(Account account)
{
	connect(account, SIGNAL(protocolHandlerChanged(Account)), this, SLOT(protocolHandlerChanged(Account)));
	protocolHandlerChanged(account);
}

void MessageManagerImpl::accountRemoved(Account account)
{
	disconnect(account, SIGNAL(protocolHandlerChanged(Account)), this, SLOT(protocolHandlerChanged(Account)));
	protocolHandlerChanged(account);
}

void MessageManagerImpl::protocolHandlerChanged(Account account)
{
	if (account.protocolHandler())
	{
		auto chatService = account.protocolHandler()->chatService();
		if (!chatService)
			return;

		connect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceivedSlot(const Message &)));
		connect(chatService, SIGNAL(messageSent(const Message &)),
				this, SIGNAL(messageSent(const Message &)));
	}
}

void MessageManagerImpl::messageReceivedSlot(const Message &message)
{
	Message transformedMessage = m_messageTransformerService
			? m_messageTransformerService.data()->transform(message)
			: message;

	if (m_messageFilterService)
		if (!m_messageFilterService.data()->acceptMessage(transformedMessage))
			return;

	emit messageReceived(transformedMessage);
}

Message MessageManagerImpl::createOutgoingMessage(const Chat &chat, NormalizedHtmlString content)
{
	Message message = m_messageStorage->create();
	message.setMessageChat(chat);
	message.setType(MessageTypeSent);
	message.setMessageSender(chat.chatAccount().accountContact());
	message.setContent(std::move(content));
	message.setSendDate(QDateTime::currentDateTime());
	message.setReceiveDate(QDateTime::currentDateTime());

	return message;
}

bool MessageManagerImpl::sendMessage(const Chat &chat, NormalizedHtmlString htmlContent, bool silent)
{
	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol || !protocol->chatService())
		return false;

	Message message = createOutgoingMessage(chat, std::move(htmlContent));
	if (m_messageFilterService && !m_messageFilterService.data()->acceptMessage(message))
		return false;

	Message transformedMessage = m_messageTransformerService
			? m_messageTransformerService.data()->transform(message)
			: message;

	bool sent = protocol->chatService()->sendMessage(transformedMessage);
	if (sent && !silent)
		emit messageSent(transformedMessage);

	return sent;
}

bool MessageManagerImpl::sendRawMessage(const Chat &chat, const QByteArray &content)
{
	Protocol *protocol = chat.chatAccount().protocolHandler();
	if (!protocol || !protocol->chatService())
		return false;

	return protocol->chatService()->sendRawMessage(chat, content);
}

#include "message-manager-impl.moc"
