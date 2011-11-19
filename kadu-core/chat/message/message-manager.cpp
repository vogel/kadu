/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "message-manager.h"

MessageManager * MessageManager::Instance = 0;

MessageManager * MessageManager::instance()
{
	if (0 == Instance)
		Instance = new MessageManager();

	return Instance;
}

MessageManager::MessageManager()
{
	triggerAllAccountsRegistered();
}

MessageManager::~MessageManager()
{
	triggerAllAccountsUnregistered();
}

void MessageManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	connect(chatService, SIGNAL(messageReceived(const Message &)),
	        this, SLOT(messageReceivedSlot(const Message &)));
	connect(chatService, SIGNAL(messageSent(const Message &)),
	        this, SIGNAL(messageSent(const Message &)));
}

void MessageManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;

	ChatService *chatService = protocol->chatService();
	if (!chatService)
		return;

	disconnect(chatService, SIGNAL(messageReceived(const Message &)),
	        this, SLOT(messageReceivedSlot(const Message &)));
	disconnect(chatService, SIGNAL(messageSent(const Message &)),
	        this, SIGNAL(messageSent(const Message &)));
}

void MessageManager::messageReceivedSlot(const Message &message)
{
	addUnreadMessage(message);

	emit messageReceived(message);
}

void MessageManager::addUnreadMessage(const Message &message)
{
	UnreadMessages.insert(message.messageChat(), message);

	emit unreadMessageAdded(message);
}

void MessageManager::removeUnreadMessage(const Message &message)
{
	UnreadMessages.remove(message.messageChat(), message);

	emit unreadMessageRemoved(message);
}

void MessageManager::markAllMessagesAsRead(const Chat &chat)
{
	const QList<Message> &messages = UnreadMessages.values(chat);
	foreach (const Message &message, messages)
	{
		message.setStatus(MessageStatusRead);
		emit unreadMessageRemoved(message);
	}

	UnreadMessages.remove(chat);
}
