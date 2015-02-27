/*
 * %kadu copyright begin%
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

#include "jabber-chat-state-service.h"

#include "services/jabber-resource-service.h"
#include "jabber-protocol.h"
#include "jid.h"

#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "protocols/services/chat-state.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppMessage.h>

namespace {

ChatState xmppStateToState(QXmppMessage::State state)
{
	switch (state)
	{
		case QXmppMessage::State::None:
			return ChatState::None;
		case QXmppMessage::State::Active:
			return ChatState::Active;
		case QXmppMessage::State::Composing:
			return ChatState::Composing;
		case QXmppMessage::State::Paused:
			return ChatState::Paused;
		case QXmppMessage::State::Inactive:
			return ChatState::Inactive;
		case QXmppMessage::State::Gone:
			return ChatState::Gone;
		default:
			return ChatState::None;
	}
}

QXmppMessage::State stateToXmppState(ChatState state)
{
	switch (state)
	{
		case ChatState::None:
			return QXmppMessage::State::None;
		case ChatState::Active:
			return QXmppMessage::State::Active;
		case ChatState::Composing:
			return QXmppMessage::State::Composing;
		case ChatState::Paused:
			return QXmppMessage::State::Paused;
		case ChatState::Inactive:
			return QXmppMessage::State::Inactive;
		case ChatState::Gone:
			return QXmppMessage::State::Gone;
		default:
			return QXmppMessage::State::None;
	}
}

}

JabberChatStateService::JabberChatStateService(QXmppClient *client, Account account, QObject *parent) :
		ChatStateService{account, parent},
		m_client{client}
{
}

JabberChatStateService::~JabberChatStateService()
{
}

void JabberChatStateService::setResourceService(JabberResourceService *resourceService)
{
	m_resourceService = resourceService;
}

void JabberChatStateService::extractReceivedChatState(const QXmppMessage &message)
{
	auto jid = Jid::parse(message.from());
	auto contact = ContactManager::instance()->byId(account(), jid.bare(), ActionCreateAndAdd);

	contact.addProperty("jabber:received-chat-state", static_cast<int>(message.state()), CustomProperties::NonStorable);
	emit peerStateChanged(contact, xmppStateToState(message.state()));
}

QXmppMessage JabberChatStateService::withSentChatState(QXmppMessage message)
{
	auto jid = Jid::parse(message.to());
	auto contact = ContactManager::instance()->byId(account(), jid.bare(), ActionCreateAndAdd);

	message.setState(QXmppMessage::State::Active);
	contact.addProperty("jabber:sent-chat-state", static_cast<int>(QXmppMessage::State::Active), CustomProperties::NonStorable);
	return message;
}

void JabberChatStateService::sendState(const Contact &contact, ChatState state)
{
	if (!m_client || !m_client->isConnected())
		return;

	if (!contact || contact.currentStatus().isDisconnected())
		return;

	auto receivedChatState = static_cast<QXmppMessage::State>(contact.property("jabber:received-chat-state", QXmppMessage::State::None).toInt());
	if (receivedChatState == QXmppMessage::State::None || receivedChatState == QXmppMessage::State::Gone)
		return;

	auto jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails || !jabberAccountDetails->sendTypingNotification())
		return;

	auto xmppState = stateToXmppState(state);
	if (!jabberAccountDetails->sendGoneNotification() && (xmppState == QXmppMessage::State::Gone || xmppState == QXmppMessage::State::Inactive))
		xmppState = QXmppMessage::State::Paused;

	auto sentChatState = static_cast<QXmppMessage::State>(contact.property("jabber:sent-chat-state", QXmppMessage::State::None).toInt());
	// invalid transition
	if (sentChatState == QXmppMessage::State::None && (xmppState != QXmppMessage::State::Active && xmppState != QXmppMessage::State::Composing && xmppState != QXmppMessage::State::Gone))
		return;

	// don't send if it is the same as last sent state
	if (xmppState == sentChatState || xmppState == QXmppMessage::State::None)
		return;

	auto jid = m_resourceService->bestContactJid(contact);

	auto xmppMessage = QXmppMessage{};
	xmppMessage.setFrom(m_client.data()->clientPresence().id());
	xmppMessage.setStamp(QDateTime::currentDateTime());
	xmppMessage.setTo(jid.full());
	xmppMessage.setType(QXmppMessage::Chat);

	if (xmppState == QXmppMessage::State::Inactive && sentChatState == QXmppMessage::State::Composing)
	{
		// send intermediate state first
		xmppMessage.setState(QXmppMessage::State::Paused);
		m_client->sendPacket(xmppMessage);
	}

	if (xmppState == QXmppMessage::State::Composing && sentChatState == QXmppMessage::State::Inactive)
	{
		// send intermediate state first
		xmppMessage.setState(QXmppMessage::State::Active);
		m_client->sendPacket(xmppMessage);
	}

	xmppMessage.setState(xmppState);
	m_client->sendPacket(xmppMessage);

	// Save last state
	// if (sentChatState != QXmppMessage::State::Gone || xmppState == QXmppMessage::State::Active) I don't know why we have this condition
	contact.addProperty("jabber:sent-chat-state", static_cast<int>(xmppState), CustomProperties::NonStorable);
}

#include "moc_jabber-chat-state-service.cpp"
