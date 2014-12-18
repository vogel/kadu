/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "resource/jabber-resource-pool.h"
#include "iris-status-adapter.h"
#include "jabber-protocol.h"

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"

namespace XMPP
{

JabberRoomChatService::JabberRoomChatService(Account account, QObject *parent) :
		AccountService{account, parent}
{
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
	connect(chatManager, SIGNAL(chatOpened(Chat)), this, SLOT(chatOpened(Chat)));
	connect(chatManager, SIGNAL(chatClosed(Chat)), this, SLOT(chatClosed(Chat)));
}

void JabberRoomChatService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberRoomChatService::setXmppClient(Client *xmppClient)
{
	m_client = xmppClient;

	connect(m_client.data(), SIGNAL(groupChatJoined(Jid)), this, SLOT(groupChatJoined(Jid)));
	connect(m_client.data(), SIGNAL(groupChatLeft(Jid)), this, SLOT(groupChatLeft(Jid)));
	connect(m_client.data(), SIGNAL(groupChatPresence(Jid,Status)), this, SLOT(groupChatPresence(Jid,Status)));
}

ChatDetailsRoom * JabberRoomChatService::myRoomChatDetails(const Chat &chat) const
{
	if (chat.chatAccount() != account())
		return nullptr;

	return qobject_cast<ChatDetailsRoom *>(chat.details());
}

void JabberRoomChatService::chatOpened(const Chat &chat)
{
	auto details = myRoomChatDetails(chat);
	if (!details)
		return;

	if (m_openedRoomChats.contains(details->room()))
		return;

	m_openedRoomChats.insert(details->room(), chat);

	auto jid = Jid{details->room()};
	m_client.data()->groupChatJoin(jid.domain(), jid.node(), details->nick());
}

void JabberRoomChatService::chatClosed(const Chat &chat)
{
	auto details = myRoomChatDetails(chat);
	if (!details || !details->stayInRoomAfterClosingWindow())
		leaveChat(chat);
}

void JabberRoomChatService::leaveChat(const Chat &chat)
{
	auto protocol = qobject_cast<XMPP::JabberProtocol *>(account().protocolHandler());
	if (protocol)
		protocol->resourcePool()->removeAllResources(chat.contacts().toContact().id());

	auto details = myRoomChatDetails(chat);
	if (!details)
		return;

	m_openedRoomChats.remove(details->room());
	m_closedRoomChats.insert(details->room(), chat);

	auto jid = Jid{details->room()};
	m_client.data()->groupChatLeave(jid.domain(), jid.node());
}

void JabberRoomChatService::groupChatJoined(const Jid &jid)
{
	auto chatId = jid.bare();
	if (!m_openedRoomChats.contains(chatId))
		return;

	auto chat = m_openedRoomChats.value(chatId);
	auto details = myRoomChatDetails(chat);
	if (details)
		details->setConnected(true);
}

void JabberRoomChatService::groupChatLeft(const Jid &jid)
{
	auto chatId = jid.bare();

	if (!m_closedRoomChats.contains(chatId) && !m_openedRoomChats.contains(chatId))
		return;

	auto chat = m_closedRoomChats.contains(chatId)
		? m_closedRoomChats.value(chatId)
		: m_openedRoomChats.value(chatId);

	auto details = myRoomChatDetails(chat);
	if (details)
	{
		details->setConnected(false);

		auto contacts = details->contacts();
		for (auto &&contact : contacts)
			details->removeContact(contact);
	}

	m_closedRoomChats.remove(chatId);
}

void JabberRoomChatService::groupChatPresence(const Jid &jid, const Status &status)
{
	auto chat = m_openedRoomChats.value(jid.bare());

	auto chatDetails = qobject_cast<ChatDetailsRoom *>(chat.details());
	if (!chatDetails)
		return;

	auto contactStatus = IrisStatusAdapter::fromIrisStatus(status);
	auto contact = m_contactManager->byId(account(), jid.full(), ActionCreateAndAdd);

	if (!contactStatus.isDisconnected())
	{
		auto buddy = m_buddyManager->byContact(contact, ActionCreateAndAdd);
		buddy.setDisplay(jid.resource());
		buddy.setTemporary(true);
	}

	contact.setCurrentStatus(contactStatus);

	if (contactStatus.isDisconnected())
		chatDetails->removeContact(contact);
	else
		chatDetails->addContact(contact);
}

bool JabberRoomChatService::isRoomChat(const Chat &chat) const
{
	return myRoomChatDetails(chat) != nullptr;
}

bool JabberRoomChatService::shouldHandleReceivedMessage(const Message& msg) const
{
	return m_openedRoomChats.contains(msg.from().bare());
}

::Message JabberRoomChatService::handleReceivedMessage(const Message &msg) const
{
	if (!m_openedRoomChats.contains(msg.from().bare()))
		return ::Message{};

	auto chat = m_openedRoomChats.value(msg.from().bare());
	auto details = myRoomChatDetails(chat);

	if (!details)
		return ::Message{};

	if (msg.from().resource() == details->nick()) // message from myself
		return ::Message{};

	auto contact = ContactManager::instance()->byId(account(), msg.from().full(), ActionCreateAndAdd);
	auto buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(msg.from().resource());
	buddy.setTemporary(true);

	::Message result = ::Message::create();
	result.setMessageChat(chat);
	result.setMessageSender(contact);

	return result;
}

}

#include "moc_jabber-room-chat-service.cpp"
