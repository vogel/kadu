/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-room-chat.h"

#include "services/jabber-presence-service.h"
#include "jid.h"

#include "buddies/buddy-manager.h"
#include "chat/chat-details-room.h"
#include "contacts/contact-manager.h"

#include <qxmpp/QXmppMucManager.h>

JabberRoomChat::JabberRoomChat(QXmppMucRoom *room, Chat chat, QObject *parent) :
		QObject{parent},
		m_room{room},
		m_chat{std::move(chat)}
{
	Q_ASSERT(nullptr != dynamic_cast<ChatDetailsRoom *>(m_chat.details()));

	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	connect(details, SIGNAL(updated()), this, SLOT(updated()));
	updated();

	connect(m_room, SIGNAL(joined()), this, SLOT(joined()));
	connect(m_room, SIGNAL(left()), this, SLOT(left()));
	connect(m_room, SIGNAL(participantAdded(QString)), this, SLOT(participantChanged(QString)));
	connect(m_room, SIGNAL(participantChanged(QString)), this, SLOT(participantChanged(QString)));
	connect(m_room, SIGNAL(participantRemoved(QString)), this, SLOT(participantRemoved(QString)));
}

JabberRoomChat::~JabberRoomChat()
{
	if (m_room)
		m_room->deleteLater();
}

void JabberRoomChat::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void JabberRoomChat::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void JabberRoomChat::setPresenceService(JabberPresenceService *presenceService)
{
	m_presenceService = presenceService;
}

bool JabberRoomChat::stayInRoomAfterClosingWindow() const
{
	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	return details->stayInRoomAfterClosingWindow();
}

void JabberRoomChat::updated()
{
	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	m_room->setNickName(details->nick());
	m_room->setPassword(details->password());
}

void JabberRoomChat::join()
{
	m_room->join();
}

void JabberRoomChat::leave()
{
	m_room->leave();
}

void JabberRoomChat::joined()
{
	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	details->setConnected(true);

	emit joined(m_chat);
}

void JabberRoomChat::left()
{
	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	details->setConnected(false);

	emit left(m_chat);
}

void JabberRoomChat::participantChanged(const QString &id)
{
	auto jid = Jid::parse(id);
	auto contact = m_contactManager->byId(m_chat.chatAccount(), id, ActionCreateAndAdd);
	auto buddy = m_buddyManager->byContact(contact, ActionCreateAndAdd);
	buddy.setDisplay(jid.resource());
	buddy.setTemporary(true);

	auto status = m_presenceService->presenceToStatus(m_room->participantPresence(id));
	contact.setCurrentStatus(status);

	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	if (status.isDisconnected())
		details->removeContact(contact);
	else
		details->addContact(contact);
}

void JabberRoomChat::participantRemoved(const QString &id)
{
	auto jid = Jid::parse(id);
	auto contact = m_contactManager->byId(m_chat.chatAccount(), id, ActionReturnNull);

	auto details = static_cast<ChatDetailsRoom *>(m_chat.details());
	details->removeContact(contact);
}

Chat JabberRoomChat::chat() const
{
	return m_chat;
}

QString JabberRoomChat::nick() const
{
	return m_room->nickName();
}

#include "moc_jabber-room-chat.cpp"
