/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "jabber-protocol.h"

#include "jabber-chat-state-service.h"

JabberChatStateService::JabberChatStateService(JabberProtocol *parent) :
		Protocol(parent)
{
	connect(Protocol->client(), SIGNAL(messageReceived(const XMPP::Message &)),
			this, SLOT(incomingMessage(const XMPP::Message &)));
	connect(Protocol->client(), SIGNAL(messageAboutToSend(XMPP::Message &)),
			this, SLOT(messageAboutToSend(XMPP::Message &)));
}

bool JabberChatStateService::shouldSendEvent(const Chat &chat)
{
	ChatInfo &info = ChatInfos[chat];
	if (!info.UserRequestedEvents && info.ContactChatState == XMPP::StateNone)
		return false;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(Protocol->account().details());
	if (!jabberAccountDetails)
		return false;

	if (!jabberAccountDetails->sendTypingNotification())
		return false;

	// Don't send to offline resource
	Contact contact = chat.contacts().toContact();
	if (contact.currentStatus().isDisconnected())
	{
		info.UserRequestedEvents = false;
		info.LastChatState = XMPP::StateNone;
		return false;
	}

	return true;
}

void JabberChatStateService::setChatState(const Chat &chat, XMPP::ChatState state)
{
	if (!shouldSendEvent(chat))
		return;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(Protocol->account().details());
	if (!jabberAccountDetails->sendGoneNotification() && (state == XMPP::StateGone || state == XMPP::StateInactive))
		state = XMPP::StatePaused;

	ChatInfo &info = ChatInfos[chat];
	//this isn't a valid transition, so don't send it, and don't update laststate
	if (info.LastChatState == XMPP::StateNone && (state != XMPP::StateActive && state != XMPP::StateComposing && state != XMPP::StateGone))
		return;

	// Check if we should send a message
	if (state == info.LastChatState || state == XMPP::StateActive || (info.LastChatState == XMPP::StateActive && state == XMPP::StatePaused))
	{
		info.LastChatState = state;
		return;
	}

	// Build event message
	XMPP::Message m(chat.contacts().toContact().id());
	if (info.UserRequestedEvents)
	{
		m.setEventId(info.EventId);
		if (state == XMPP::StateComposing)
			m.addEvent(XMPP::ComposingEvent);
		else if (info.LastChatState == XMPP::StateComposing)
			m.addEvent(XMPP::CancelEvent);
	}

	if (info.ContactChatState != XMPP::StateNone)
	{
		if (info.LastChatState != XMPP::StateGone)
		{
			if ((state == XMPP::StateInactive && info.LastChatState == XMPP::StateComposing)
				|| (state == XMPP::StateComposing && info.LastChatState == XMPP::StateInactive))
			{
				// First go to the paused state
				XMPP::Message tm(chat.contacts().toContact().id());
				tm.setType("chat");
				tm.setChatState(XMPP::StatePaused);

				if (Protocol->isConnected())
					Protocol->client()->client()->sendMessage(tm);
			}
			m.setChatState(state);
		}
	}

	// Send event message
	if (m.containsEvents() || m.chatState() != XMPP::StateNone)
	{
		m.setType("chat");
		if (Protocol->isConnected())
			Protocol->client()->client()->sendMessage(m);
	}

	// Save last state
	if (info.LastChatState != XMPP::StateGone || state == XMPP::StateActive)
		info.LastChatState = state;
}

ChatStateService::ContactActivity JabberChatStateService::xmppStateToContactState(XMPP::ChatState state)
{
	switch (state)
	{
		case XMPP::StateNone:
			return StateNone;
		case XMPP::StateActive:
			return StateActive;
		case XMPP::StateComposing:
			return StateComposing;
		case XMPP::StatePaused:
			return StatePaused;
		case XMPP::StateInactive:
			return StateInactive;
		case XMPP::StateGone:
			return StateGone;
		default:
			return StateNone;
	}
}

void JabberChatStateService::incomingMessage(const XMPP::Message &msg)
{
	Contact contact = ContactManager::instance()->byId(Protocol->account(), msg.from().bare(), ActionCreateAndAdd);
	Chat chat = ChatManager::instance()->findChat(ContactSet(contact), true);
	ChatInfo &info = ChatInfos[chat];

	if (msg.body().isEmpty())
	{
		// Event message
		if (msg.containsEvent(XMPP::CancelEvent))
		{
			info.ContactChatState = XMPP::StatePaused;
			emit contactActivityChanged(StatePaused, contact);
		}
		else if (msg.containsEvent(XMPP::ComposingEvent))
		{
			info.ContactChatState = XMPP::StateComposing;
			emit contactActivityChanged(StateComposing, contact);
		}

		if (msg.chatState() != XMPP::StateNone)
		{
			info.ContactChatState = msg.chatState();
			emit contactActivityChanged(xmppStateToContactState(msg.chatState()), contact);
		}
	}
	else
	{
		// Normal message
		// Check if user requests event messages XEP22
		info.UserRequestedEvents = msg.containsEvent(XMPP::ComposingEvent);

		if (!msg.eventId().isEmpty())
			info.EventId = msg.eventId();

		if (msg.containsEvents() || msg.chatState() != XMPP::StateNone)
		{
			info.ContactChatState = XMPP::StateActive;
			emit contactActivityChanged(StateActive, contact);
		}
		else
		{
			info.ContactChatState = XMPP::StateNone;
			emit contactActivityChanged(StateNone, contact);
		}
	}
}

void JabberChatStateService::messageAboutToSend(XMPP::Message &message)
{
	message.addEvent(XMPP::ComposingEvent);
	message.setChatState(XMPP::StateActive);
}

void JabberChatStateService::composingStarted(const Chat &chat)
{
	setChatState(chat, XMPP::StateComposing);
}

void JabberChatStateService::composingStopped(const Chat &chat)
{
	setChatState(chat, XMPP::StatePaused);
}

void JabberChatStateService::chatWidgetClosed(const Chat &chat)
{
	setChatState(chat, XMPP::StateGone);
	ChatInfos.remove(chat);
}

void JabberChatStateService::chatWidgetActivated(const Chat &chat)
{
	setChatState(chat, XMPP::StateActive);
}

void JabberChatStateService::chatWidgetDeactivated(const Chat &chat)
{
	setChatState(chat, XMPP::StateInactive);
}

