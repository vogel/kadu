/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "unread-message-repository.h"

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-buddy.h"
#include "message/message.h"
#include "message/sorted-messages.h"

UnreadMessageRepository::UnreadMessageRepository(QObject *parent) :
		QObject{parent}
{
	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);
}

UnreadMessageRepository::~UnreadMessageRepository()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

bool UnreadMessageRepository::importFromPendingMessages()
{
	auto pendingMessagesNode = xml_config_file->getNode("PendingMessages", XmlConfigFile::ModeFind);
	if (pendingMessagesNode.isNull())
		return false;

	auto messageElements = xml_config_file->getNodes(pendingMessagesNode, "Message");
	for (const auto &messageElement : messageElements)
	{
		auto storagePoint = std::make_shared<StoragePoint>(storage()->storage(), messageElement);
		auto uuid = QUuid{storagePoint->point().attribute("uuid")};
		if (!uuid.isNull())
		{
			auto message = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(message);

			// reset storage for message as it will be stored in other place
			message.data()->setStorage({});
			message.data()->setState(StateNew);
		}
	}

	// PendingMessages is no longer needed
	pendingMessagesNode.parentNode().removeChild(pendingMessagesNode);

	return true;
}

void UnreadMessageRepository::load()
{
	StorableObject::load();

	if (importFromPendingMessages())
	{
		loaded();
		return;
	}

	auto itemsNode = storage()->point();
	if (itemsNode.isNull())
		return;

	auto itemElements = storage()->storage()->getNodes(itemsNode, "Message");
	m_unreadMessages.reserve(itemElements.count());

	for (const auto &itemElement : itemElements)
	{
		auto storagePoint = std::make_shared<StoragePoint>(storage()->storage(), itemElement);
		auto uuid = QUuid{storagePoint->point().attribute("uuid")};
		if (!uuid.isNull())
		{
			auto item = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(item);
		}
	}

	loaded();
}

void UnreadMessageRepository::store()
{
	ensureLoaded();

	for (auto message : m_unreadMessages)
		message.ensureStored();
}

void UnreadMessageRepository::addUnreadMessage(const Message &message)
{
	// just ensure that owner buddy is managed - we need it to be shown on contact list
	// todo: rethink this one
	BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

	m_unreadMessages.append(message);

	emit unreadMessageAdded(message);
}

void UnreadMessageRepository::removeUnreadMessage(const Message &message)
{
	m_unreadMessages.removeAll(message);
	message.data()->removeFromStorage();

	emit unreadMessageRemoved(message);
}

const QList<Message> & UnreadMessageRepository::allUnreadMessages() const
{
	return m_unreadMessages;
}

SortedMessages UnreadMessageRepository::unreadMessagesForChat(const Chat &chat) const
{
	auto chats = QSet<Chat>{};

	auto details = chat.details();
	auto chatDetailsBuddy = qobject_cast<ChatDetailsBuddy *>(details);

	if (chatDetailsBuddy)
		for (const auto &ch : chatDetailsBuddy->chats())
			chats.insert(ch);
	else
		chats.insert(chat);

	auto messages = std::vector<Message>{};
	std::copy_if(std::begin(m_unreadMessages), std::end(m_unreadMessages), std::back_inserter(messages),
			[&chats](const Message &message){ return chats.contains(message.messageChat()); }
	);

	return SortedMessages{messages};
}

bool UnreadMessageRepository::hasUnreadMessages() const
{
	return !m_unreadMessages.isEmpty();
}

int UnreadMessageRepository::unreadMessagesCount() const
{
	return m_unreadMessages.count();
}

void UnreadMessageRepository::markMessagesAsRead(const SortedMessages &messages)
{
	for (const auto &message : messages)
		if (m_unreadMessages.removeAll(message) > 0)
		{
			message.data()->removeFromStorage();
			emit unreadMessageRemoved(message);
		}
}

Message UnreadMessageRepository::unreadMessage() const
{
	if (m_unreadMessages.empty())
		return Message::null;
	else
		return m_unreadMessages.at(0);
}

Message UnreadMessageRepository::unreadMessageForBuddy(const Buddy &buddy) const
{
	auto contacts = buddy.contacts();
	for (const auto &message : m_unreadMessages)
		if (contacts.contains(message.messageSender()))
			return message;

	return Message::null;
}

Message UnreadMessageRepository::unreadMessageForContact(const Contact &contact) const
{
	for (const auto &message : m_unreadMessages)
		if (contact == message.messageSender())
			return message;

	return Message::null;
}

#include "moc_unread-message-repository.cpp"
