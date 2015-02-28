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

#include "jabber-protocol.h"
#include "jid.h"

#include "buddies/buddy-manager.h"
#include "chat/chat-details-room.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-manager.h"

#include <qxmpp/QXmppMessage.h>
#include <qxmpp/QXmppMucManager.h>

JabberRoomChatService::JabberRoomChatService(QXmppMucManager *muc, Account account, QObject *parent) :
		AccountService{account, parent},
		m_muc{muc}
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

	auto room = m_muc->addRoom(details->room());
	connect(room, SIGNAL(joined()), this, SLOT(groupChatJoined()));
	connect(room, SIGNAL(left()), this, SLOT(groupChatLeft()));
	connect(room, SIGNAL(participantAdded(QString)), this, SLOT(participantChanged(QString)));
	connect(room, SIGNAL(participantChanged(QString)), this, SLOT(participantChanged(QString)));
	connect(room, SIGNAL(participantRemoved(QString)), this, SLOT(participantChanged(QString)));

	room->setNickName(details->nick());
	room->join();
}

void JabberRoomChatService::chatClosed(const Chat &chat)
{
	auto details = myRoomChatDetails(chat);
	if (!details || !details->stayInRoomAfterClosingWindow())
		leaveChat(chat);
}

void JabberRoomChatService::leaveChat(const Chat &chat)
{
	auto details = myRoomChatDetails(chat);
	if (!details)
		return;

	m_openedRoomChats.remove(details->room());
	m_closedRoomChats.insert(details->room(), chat);

	auto rooms = m_muc->rooms();
	auto roomIt = std::find_if(std::begin(rooms), std::end(rooms), [&details](QXmppMucRoom *room){
		return room->jid() == details->room();
	});

	if (roomIt == std::end(rooms))
		return;

	auto room = *roomIt;
	room->leave();
	room->deleteLater();
}

void JabberRoomChatService::groupChatJoined()
{
	auto room = qobject_cast<QXmppMucRoom *>(sender());
	if (!room)
		return;

	auto jid = Jid::parse(room->jid());
	if (!m_openedRoomChats.contains(jid.bare()))
		return;

	auto chat = m_openedRoomChats.value(jid.bare());
	auto details = myRoomChatDetails(chat);
	if (details)
		details->setConnected(true);
}

void JabberRoomChatService::groupChatLeft()
{
	auto room = qobject_cast<QXmppMucRoom *>(sender());
	if (!room)
		return;

	auto jid = Jid::parse(room->jid());
	if (!m_closedRoomChats.contains(jid.bare()) && !m_openedRoomChats.contains(jid.bare()))
		return;

	auto chat = m_closedRoomChats.contains(jid.bare())
		? m_closedRoomChats.value(jid.bare())
		: m_openedRoomChats.value(jid.bare());

	auto details = myRoomChatDetails(chat);
	if (details)
	{
		details->setConnected(false);

		auto contacts = details->contacts();
		for (auto &&contact : contacts)
			details->removeContact(contact);
	}

	m_closedRoomChats.remove(jid.bare());
}

void JabberRoomChatService::participantChanged(const QString &id)
{
	auto room = qobject_cast<QXmppMucRoom *>(sender());
	if (!room)
		return;

	auto jid = Jid::parse(id);
	auto chat = m_openedRoomChats.value(jid.bare());

	auto chatDetails = qobject_cast<ChatDetailsRoom *>(chat.details());
	if (!chatDetails)
		return;

	auto contact = m_contactManager->byId(account(), id, ActionCreateAndAdd);
	auto presence = room->participantPresence(id);
	auto buddy = m_buddyManager->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(jid.resource());
	buddy.setTemporary(true);

	auto status = Status{};
	if (presence.type() == QXmppPresence::Available)
	{
		switch (presence.availableStatusType())
		{
			case QXmppPresence::AvailableStatusType::Online:
				status.setType(StatusTypeOnline);
				break;
			case QXmppPresence::AvailableStatusType::Away:
				status.setType(StatusTypeAway);
				break;
			case QXmppPresence::AvailableStatusType::XA:
				status.setType(StatusTypeNotAvailable);
				break;
			case QXmppPresence::AvailableStatusType::DND:
				status.setType(StatusTypeDoNotDisturb);
				break;
			case QXmppPresence::AvailableStatusType::Chat:
				status.setType(StatusTypeFreeForChat);
				break;
			case QXmppPresence::AvailableStatusType::Invisible:
				status.setType(StatusTypeDoNotDisturb);
				break;
		}
	}
	else if (presence.type() == QXmppPresence::Unavailable)
		status.setType(StatusTypeOffline);

	status.setDescription(presence.statusText());

	contact.setCurrentStatus(status);
	if (status.isDisconnected())
		chatDetails->removeContact(contact);
	else
		chatDetails->addContact(contact);
}

bool JabberRoomChatService::isRoomChat(const Chat &chat) const
{
	return myRoomChatDetails(chat) != nullptr;
}

bool JabberRoomChatService::shouldHandleReceivedMessage(const QXmppMessage &xmppMessage) const
{
	auto jid = Jid::parse(xmppMessage.from());
	return m_openedRoomChats.contains(jid.bare());
}

Message JabberRoomChatService::handleReceivedMessage(const QXmppMessage &xmppMessage) const
{
	auto jid = Jid::parse(xmppMessage.from());
	if (!m_openedRoomChats.contains(jid.bare()))
		return Message::null;

	auto chat = m_openedRoomChats.value(jid.bare());
	auto details = myRoomChatDetails(chat);

	if (!details)
		return Message::null;

	if (jid.resource() == details->nick()) // message from myself
		return Message::null;

	auto contact = ContactManager::instance()->byId(account(), jid.full(), ActionCreateAndAdd);
	auto buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(jid.resource());
	buddy.setTemporary(true);

	auto result = Message::create();
	result.setMessageChat(chat);
	result.setMessageSender(contact);

	return result;
}

#include "moc_jabber-room-chat-service.cpp"
