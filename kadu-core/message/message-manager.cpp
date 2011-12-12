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
#include "buddies/buddy-manager.h"
#include "chat/chat-details-aggregate.h"
#include "configuration/xml-configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include "message-manager.h"

MessageManager * MessageManager::Instance = 0;

MessageManager * MessageManager::instance()
{
	if (0 == Instance)
	{
		Instance = new MessageManager();
		Instance->init();
	}

	return Instance;
}

MessageManager::MessageManager()
{
}

MessageManager::~MessageManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);

	triggerAllAccountsUnregistered();
}

void MessageManager::init()
{
	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);

	triggerAllAccountsRegistered();
}

bool MessageManager::importFromPendingMessages()
{
	QDomElement pendingMessagesNode = xml_config_file->getNode("PendingMessages", XmlConfigFile::ModeFind);
	if (pendingMessagesNode.isNull())
		return false;

	QVector<QDomElement> messageElements = xml_config_file->getNodes(pendingMessagesNode, "Message");
	foreach (const QDomElement &messageElement, messageElements)
	{
		QSharedPointer<StoragePoint> storagePoint(new StoragePoint(storage()->storage(), messageElement));
		QUuid uuid = storagePoint->point().attribute("uuid");
		if (!uuid.isNull())
		{
			Message message = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(message);

			// reset storage for message as it will be stored in other place
			message.data()->setStorage(QSharedPointer<StoragePoint>());
			message.data()->setState(StateNew);
		}
	}

	// PendingMessages is no longer needed
	pendingMessagesNode.parentNode().removeChild(pendingMessagesNode);

	return true;
}

void MessageManager::load()
{
	StorableObject::load();

	if (importFromPendingMessages())
	{
		loaded();
		return;
	}

	QDomElement itemsNode = storage()->point();
	if (itemsNode.isNull())
		return;

	QVector<QDomElement> itemElements = storage()->storage()->getNodes(itemsNode, "Message");
	UnreadMessages.reserve(itemElements.count());

	foreach (const QDomElement &itemElement, itemElements)
	{
		QSharedPointer<StoragePoint> storagePoint(new StoragePoint(storage()->storage(), itemElement));
		QUuid uuid = storagePoint->point().attribute("uuid");
		if (!uuid.isNull())
		{
			Message item = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(item);
		}
	}

	loaded();
}

void MessageManager::store()
{
	ensureLoaded();

	foreach (Message message, UnreadMessages)
		message.ensureStored();
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
	// just ensure that owner buddy is managed - we need it to be shown on contact list
	// todo: rethink this one
	BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat(), false);
	// message is pending if chat widget is not open
	if (!chatWidget)
		message.setPending(true);
	else if (chatWidget->isActive())
		return;

	UnreadMessages.append(message);
	emit unreadMessageAdded(message);
}

void MessageManager::removeUnreadMessage(const Message &message)
{
	UnreadMessages.removeAll(message);

	message.setPending(false);
	message.data()->removeFromStorage();

	emit unreadMessageRemoved(message);
}

const QList<Message> & MessageManager::allUnreadMessages() const
{
	return UnreadMessages;
}

QList<Message> MessageManager::chatUnreadMessages(const Chat &chat) const
{
	QList<Message> result;
	QSet<Chat> chats;

	ChatDetails *details = chat.details();
	ChatDetailsAggregate *aggregateDetails = qobject_cast<ChatDetailsAggregate *>(details);

	if (aggregateDetails)
		foreach (const Chat &ch, aggregateDetails->chats())
			chats.insert(ch);
	else
		chats.insert(chat);

	foreach (const Message &message, UnreadMessages)
		if (chats.contains(message.messageChat()))
			result.append(message);

	return result;
}

bool MessageManager::hasUnreadMessages() const
{
	return !UnreadMessages.isEmpty();
}

quint16 MessageManager::unreadMessagesCount() const
{
	return UnreadMessages.count();
}

void MessageManager::markAllMessagesAsRead(const Chat &chat)
{
	const QList<Message> &messages = chatUnreadMessages(chat);

	foreach (const Message &message, messages)
	{
		UnreadMessages.removeAll(message);

		message.setStatus(MessageStatusRead);
		message.setPending(false);
		message.data()->removeFromStorage();

		emit unreadMessageRemoved(message);
	}
}

Message MessageManager::unreadMessage() const
{
	if (UnreadMessages.empty())
		return Message::null;
	else
		return UnreadMessages.at(0);
}

Message MessageManager::unreadMessageForBuddy(const Buddy &buddy) const
{
	const QList<Contact> &contacts = buddy.contacts();
	foreach (const Message &message, UnreadMessages)
		if (contacts.contains(message.messageSender()))
			return message;

	return Message::null;
}

Message MessageManager::unreadMessageForContact(const Contact &contact) const
{
	foreach (const Message &message, UnreadMessages)
		if (contact == message.messageSender())
			return message;

	return Message::null;
}
