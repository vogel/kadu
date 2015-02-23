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

#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "jabber-protocol.h"

#include "jabber-chat-state-service.h"

JabberChatStateService::JabberChatStateService(Account account, QObject *parent) :
		ChatStateService(account, parent)
{
}

JabberChatStateService::~JabberChatStateService()
{
}

bool JabberChatStateService::shouldSendEvent(const Contact &contact)
{
	Q_UNUSED(contact);
	return false;
	/*
	if (!contact)
		return false;

	ContactInfo &info = ContactInfos[contact];
	if (!info.UserRequestedEvents && info.ContactChatState == StateNone)
		return false;

	// Don't send to offline resource
	if (contact.currentStatus().isDisconnected())
	{
		info.UserRequestedEvents = false;
		info.LastChatState = StateNone;
		return false;
	}

	if (info.ContactChatState == StateGone)
		return false;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return false;

	if (!jabberAccountDetails->sendTypingNotification())
		return false;

	return true;*/
}
/*
void JabberChatStateService::setChatState(const Contact &contact, ChatState state)
{
	if (!XmppClient)
		return;

	if (!shouldSendEvent(contact))
		return;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (jabberAccountDetails && !jabberAccountDetails->sendGoneNotification() && (state == StateGone || state == StateInactive))
		state = StatePaused;

	ContactInfo &info = ContactInfos[contact];
	//this isn't a valid transition, so don't send it, and don't update laststate
	if (info.LastChatState == StateNone && (state != StateActive && state != StateComposing && state != StateGone))
		return;

	// Check if we should send a message
	if (state == info.LastChatState ||
			(state == StateActive && info.LastChatState == StatePaused) ||
			(info.LastChatState == StateActive && state == StatePaused))
		return;

	// Build event message
	Message m(contact.id());
	if (info.UserRequestedEvents)
	{
		m.setEventId(info.EventId);
		if (state == StateComposing)
			m.addEvent(ComposingEvent);
		else if (info.LastChatState == StateComposing)
			m.addEvent(CancelEvent);
	}

	if (info.ContactChatState != StateNone)
	{
		if (info.LastChatState != StateGone)
		{
			if ((state == StateInactive && info.LastChatState == StateComposing)
				|| (state == StateComposing && info.LastChatState == StateInactive))
			{
				// First go to the paused or active state
				Message tm(contact.id());
				tm.setType("chat");
				tm.setChatState(info.LastChatState == StateComposing
						? StatePaused
						: StateActive);

				if (XmppClient.data()->isActive())
					XmppClient.data()->sendMessage(tm);
			}
			m.setChatState(state);
		}
	}

	// Send event message
	if (m.containsEvents() || m.chatState() != StateNone)
	{
		m.setType("chat");
		if (XmppClient.data()->isActive())
			XmppClient.data()->sendMessage(m);
	}

	// Save last state
	if (info.LastChatState != StateGone || state == StateActive)
		info.LastChatState = state;
}

ChatStateService::State JabberChatStateService::xmppStateToContactState(ChatState state)
{
	switch (state)
	{
		case StateNone:
			return StateNone;
		case StateActive:
			return StateActive;
		case StateComposing:
			return StateComposing;
		case StatePaused:
			return StatePaused;
		case StateInactive:
			return StateInactive;
		case StateGone:
			return StateGone;
		default:
			return StateNone;
	}
}

void JabberChatStateService::handleReceivedMessage(const Message &msg)
{
	Contact contact = ContactManager::instance()->byId(account(), msg.from().bare(), ActionCreateAndAdd);
	ContactInfo &info = ContactInfos[contact];

	if (msg.body().isEmpty())
	{
		// Event message
		if (msg.containsEvent(CancelEvent))
		{
			info.ContactChatState = StatePaused;
			emit peerStateChanged(contact, StatePaused);
		}
		else if (msg.containsEvent(ComposingEvent))
		{
			info.ContactChatState = StateComposing;
			emit peerStateChanged(contact, StateComposing);
		}

		if (msg.chatState() != StateNone)
		{
			info.ContactChatState = msg.chatState();
			emit peerStateChanged(contact, xmppStateToContactState(msg.chatState()));
		}
	}
	else
	{
		// Normal message
		// Check if user requests event messages XEP22
		info.UserRequestedEvents = msg.containsEvent(ComposingEvent);

		if (!msg.eventId().isEmpty())
			info.EventId = msg.eventId();

		if (msg.containsEvents() || msg.chatState() != StateNone)
		{
			info.ContactChatState = StateActive;
			emit peerStateChanged(contact, StateActive);
		}
		else
		{
			info.ContactChatState = StateNone;
			emit peerStateChanged(contact, StateNone);
		}
	}
}

void JabberChatStateService::handleMessageAboutToSend(Message &message)
{
	Contact contact = ContactManager::instance()->byId(account(), message.to().bare(), ActionCreateAndAdd);

	if (ContactInfos[contact].UserRequestedEvents)
		message.addEvent(ComposingEvent);

	message.setChatState(StateActive);
	ContactInfos[contact].LastChatState = StateActive;
}
*/
void JabberChatStateService::sendState(const Contact &contact, State state)
{
	Q_UNUSED(contact);
	Q_UNUSED(state);/*
	switch (state)
	{
		case StateActive:
			setChatState(contact, StateActive);
			break;
		case StateComposing:
			setChatState(contact, StateComposing);
			break;
		case StateGone:
			setChatState(contact, StateGone);
			ContactInfos.remove(contact);
			break;
		case StateInactive:
			setChatState(contact, StateInactive);
			break;
		case StatePaused:
			setChatState(contact, StatePaused);
			break;
		default:
			break;
	}*/
}

#include "moc_jabber-chat-state-service.cpp"
