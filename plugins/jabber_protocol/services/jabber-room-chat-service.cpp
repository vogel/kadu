/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-room-chat-service.h"

#include "services/jabber-presence-service.h"
#include "jabber-room-chat.h"
#include "jabber-protocol.h"
#include "jid.h"

#include "buddies/buddy-manager.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-manager.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppMessage.h>
#include <qxmpp/QXmppMucManager.h>

JabberRoomChatService::JabberRoomChatService(QXmppClient *client, QXmppMucManager *muc, Account account, QObject *parent) :
		AccountService{account, parent},
		m_client{client},
		m_muc{muc}
{
	connect(m_client, SIGNAL(connected()), this, SLOT(connected()));
}

JabberRoomChatService::~JabberRoomChatService()
{
}

void JabberRoomChatService::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void JabberRoomChatService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
	connect(m_chatManager, SIGNAL(chatOpened(Chat)), this, SLOT(chatOpened(Chat)));
	connect(m_chatManager, SIGNAL(chatClosed(Chat)), this, SLOT(chatClosed(Chat)));
}

void JabberRoomChatService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberRoomChatService::setPresenceService(JabberPresenceService *presenceService)
{
	m_presenceService = presenceService;
}

void JabberRoomChatService::initialize()
{
	for (auto &&chat : m_chatManager->chats(account()))
		if (chat.isOpen() && myRoomChatDetails(chat))
			getOrCreateRoomChat(chat);
}

ChatDetailsRoom * JabberRoomChatService::myRoomChatDetails(const Chat &chat) const
{
	if (chat.chatAccount() != account())
		return nullptr;

	return qobject_cast<ChatDetailsRoom *>(chat.details());
}

void JabberRoomChatService::connected()
{
	for (auto &&roomChat : m_chats)
		roomChat->join();
}

JabberRoomChat * JabberRoomChatService::getRoomChat(const QString &id) const
{
	for (auto &&roomChat : m_chats)
	{
		auto details = myRoomChatDetails(roomChat->chat());
		if (details && details->room() == id)
			return roomChat;
	}

	return nullptr;
}

JabberRoomChat * JabberRoomChatService::getRoomChat(const Chat &chat) const
{
	if (chat.chatAccount() != account())
		return nullptr;

	if (m_chats.contains(chat))
		return m_chats[chat];
	else
		return nullptr;
}

JabberRoomChat * JabberRoomChatService::getOrCreateRoomChat(const Chat &chat)
{
	if (chat.chatAccount() != account())
		return nullptr;

	if (m_chats.contains(chat))
		return m_chats[chat];

	auto details = qobject_cast<ChatDetailsRoom *>(chat.details());
	if (!details)
		return nullptr;

	auto room = m_muc->addRoom(details->room());
	auto roomChat = new JabberRoomChat{room, chat, this};
	roomChat->setBuddyManager(m_buddyManager);
	roomChat->setContactManager(m_contactManager);
	roomChat->setPresenceService(m_presenceService);

	m_chats[chat] = roomChat;
	return roomChat;
}

void JabberRoomChatService::chatOpened(const Chat &chat)
{
	auto roomChat = getOrCreateRoomChat(chat);
	if (!roomChat)
		return;

	if (m_client->isConnected())
		roomChat->join();
}

void JabberRoomChatService::chatClosed(const Chat &chat)
{
	auto roomChat = getRoomChat(chat);
	if (!roomChat)
		return;

	if (!roomChat->stayInRoomAfterClosingWindow())
		leaveChat(chat);
}

void JabberRoomChatService::leaveChat(const Chat &chat)
{
	auto roomChat = getRoomChat(chat);
	if (!roomChat)
		return;

	roomChat->leave();
	m_chats[chat]->deleteLater();
	m_chats.remove(chat);
}

bool JabberRoomChatService::isRoomChat(const Chat &chat) const
{
	return myRoomChatDetails(chat) != nullptr;
}

Message JabberRoomChatService::handleReceivedMessage(const QXmppMessage &xmppMessage) const
{
	auto jid = Jid::parse(xmppMessage.from());
	auto roomChat = getRoomChat(jid.bare());
	if (!roomChat)
		return Message::null;

	if (jid.resource() == roomChat->nick()) // message from myself
		return Message::null;

	auto contact = ContactManager::instance()->byId(account(), jid.full(), ActionCreateAndAdd);
	auto buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(jid.resource());
	buddy.setTemporary(true);

	auto result = Message::create();
	result.setMessageChat(roomChat->chat());
	result.setMessageSender(contact);

	return result;
}

#include "moc_jabber-room-chat-service.cpp"
