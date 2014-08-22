/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <xmpp_client.h>

#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "jabber-protocol.h"

#include "jabber-chat-state-service.h"

namespace XMPP
{

JabberChatStateService::JabberChatStateService(Account account, QObject *parent) :
		ChatStateService(account, parent)
{
}

JabberChatStateService::~JabberChatStateService()
{
}

void JabberChatStateService::setClient(Client *xmppClient)
{
	XmppClient = xmppClient;
}

bool JabberChatStateService::shouldSendEvent(const Contact &contact)
{
	if (!contact)
		return false;

	ContactInfo &info = ContactInfos[contact];
	if (!info.UserRequestedEvents && info.ContactChatState == XMPP::StateNone)
		return false;

	// Don't send to offline resource
	if (contact.currentStatus().isDisconnected())
	{
		info.UserRequestedEvents = false;
		info.LastChatState = XMPP::StateNone;
		return false;
	}

	if (info.ContactChatState == XMPP::StateGone)
		return false;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return false;

	if (!jabberAccountDetails->sendTypingNotification())
		return false;

	return true;
}

void JabberChatStateService::setChatState(const Contact &contact, XMPP::ChatState state)
{
	if (!XmppClient)
		return;

	if (!shouldSendEvent(contact))
		return;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (jabberAccountDetails && !jabberAccountDetails->sendGoneNotification() && (state == XMPP::StateGone || state == XMPP::StateInactive))
		state = XMPP::StatePaused;

	ContactInfo &info = ContactInfos[contact];
	//this isn't a valid transition, so don't send it, and don't update laststate
	if (info.LastChatState == XMPP::StateNone && (state != XMPP::StateActive && state != XMPP::StateComposing && state != XMPP::StateGone))
		return;

	// Check if we should send a message
	if (state == info.LastChatState ||
			(state == XMPP::StateActive && info.LastChatState == XMPP::StatePaused) ||
			(info.LastChatState == XMPP::StateActive && state == XMPP::StatePaused))
		return;

	// Build event message
	XMPP::Message m(contact.id());
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
				// First go to the paused or active state
				XMPP::Message tm(contact.id());
				tm.setType("chat");
				tm.setChatState(info.LastChatState == XMPP::StateComposing
						? XMPP::StatePaused
						: XMPP::StateActive);

				if (XmppClient.data()->isActive())
					XmppClient.data()->sendMessage(tm);
			}
			m.setChatState(state);
		}
	}

	// Send event message
	if (m.containsEvents() || m.chatState() != XMPP::StateNone)
	{
		m.setType("chat");
		if (XmppClient.data()->isActive())
			XmppClient.data()->sendMessage(m);
	}

	// Save last state
	if (info.LastChatState != XMPP::StateGone || state == XMPP::StateActive)
		info.LastChatState = state;
}

ChatStateService::State JabberChatStateService::xmppStateToContactState(XMPP::ChatState state)
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

void JabberChatStateService::handleReceivedMessage(const XMPP::Message &msg)
{
	Contact contact = ContactManager::instance()->byId(account(), msg.from().bare(), ActionCreateAndAdd);
	ContactInfo &info = ContactInfos[contact];

	if (msg.body().isEmpty())
	{
		// Event message
		if (msg.containsEvent(XMPP::CancelEvent))
		{
			info.ContactChatState = XMPP::StatePaused;
			emit peerStateChanged(contact, StatePaused);
		}
		else if (msg.containsEvent(XMPP::ComposingEvent))
		{
			info.ContactChatState = XMPP::StateComposing;
			emit peerStateChanged(contact, StateComposing);
		}

		if (msg.chatState() != XMPP::StateNone)
		{
			info.ContactChatState = msg.chatState();
			emit peerStateChanged(contact, xmppStateToContactState(msg.chatState()));
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
			emit peerStateChanged(contact, StateActive);
		}
		else
		{
			info.ContactChatState = XMPP::StateNone;
			emit peerStateChanged(contact, StateNone);
		}
	}
}

void JabberChatStateService::handleMessageAboutToSend(XMPP::Message &message)
{
	Contact contact = ContactManager::instance()->byId(account(), message.to().bare(), ActionCreateAndAdd);

	if (ContactInfos[contact].UserRequestedEvents)
		message.addEvent(XMPP::ComposingEvent);

	message.setChatState(XMPP::StateActive);
	ContactInfos[contact].LastChatState = XMPP::StateActive;
}

void JabberChatStateService::sendState(const Contact &contact, State state)
{
	switch (state)
	{
		case StateActive:
			setChatState(contact, XMPP::StateActive);
			break;
		case StateComposing:
			setChatState(contact, XMPP::StateComposing);
			break;
		case StateGone:
			setChatState(contact, XMPP::StateGone);
			ContactInfos.remove(contact);
			break;
		case StateInactive:
			setChatState(contact, XMPP::StateInactive);
			break;
		case StatePaused:
			setChatState(contact, XMPP::StatePaused);
			break;
		default:
			break;
	}
}

}

#include "moc_jabber-chat-state-service.cpp"
