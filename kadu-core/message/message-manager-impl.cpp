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

#include "chat/chat-service-repository.h"
#include "message/message-filter-service.h"
#include "message/message-storage.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"
#include "services/message-transformer-service.h"

MessageManagerImpl::MessageManagerImpl(QObject *parent) : MessageManager{parent}
{
}

MessageManagerImpl::~MessageManagerImpl()
{
}

void MessageManagerImpl::setChatServiceRepository(ChatServiceRepository *chatServiceRepository)
{
    m_chatServiceRepository = chatServiceRepository;
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
    connect(
        m_chatServiceRepository, &ChatServiceRepository::chatServiceAdded, this, &MessageManagerImpl::addChatService);
    connect(
        m_chatServiceRepository, &ChatServiceRepository::chatServiceRemoved, this,
        &MessageManagerImpl::removeChatService);

    for (auto chatService : m_chatServiceRepository)
        addChatService(chatService);
}

void MessageManagerImpl::done()
{
    for (auto chatService : m_chatServiceRepository)
        removeChatService(chatService);

    disconnect(
        m_chatServiceRepository, &ChatServiceRepository::chatServiceAdded, this, &MessageManagerImpl::addChatService);
    disconnect(
        m_chatServiceRepository, &ChatServiceRepository::chatServiceRemoved, this,
        &MessageManagerImpl::removeChatService);
}

void MessageManagerImpl::addChatService(ChatService *chatService)
{
    connect(chatService, &ChatService::messageReceived, this, &MessageManagerImpl::messageReceivedSlot);
    connect(chatService, &ChatService::messageSent, this, &MessageManagerImpl::messageSent);
}

void MessageManagerImpl::removeChatService(ChatService *chatService)
{
    disconnect(chatService, &ChatService::messageReceived, this, &MessageManagerImpl::messageReceivedSlot);
    disconnect(chatService, &ChatService::messageSent, this, &MessageManagerImpl::messageSent);
}

void MessageManagerImpl::messageReceivedSlot(const Message &message)
{
    Message transformedMessage =
        m_messageTransformerService ? m_messageTransformerService.data()->transform(message) : message;

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
    auto chatService = m_chatServiceRepository->chatService(chat.chatAccount());
    if (!chatService)
        return false;

    Message message = createOutgoingMessage(chat, std::move(htmlContent));
    if (m_messageFilterService && !m_messageFilterService.data()->acceptMessage(message))
        return false;

    Message transformedMessage =
        m_messageTransformerService ? m_messageTransformerService.data()->transform(message) : message;

    bool sent = chatService->sendMessage(transformedMessage);
    if (sent && !silent)
        emit messageSent(transformedMessage);

    return sent;
}

bool MessageManagerImpl::sendRawMessage(const Chat &chat, const QByteArray &content)
{
    auto chatService = m_chatServiceRepository->chatService(chat.chatAccount());
    if (chatService)
        return chatService->sendRawMessage(chat, content);
    else
        return false;
}

#include "message-manager-impl.moc"
