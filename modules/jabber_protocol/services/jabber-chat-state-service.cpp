/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "contacts/contact-set.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/message-dialog.h"

#include "jabber-account-details.h"
#include "jabber-protocol.h"

#include "jabber-chat-state-service.h"

// TODO 0.8.0: Review and cleanup

JabberChatStateService::JabberChatStateService(JabberProtocol *parent) :
		ParentProtocol(parent)
{
	foreach (const Chat &chat, ChatManager::instance()->items())
	{
		ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
		if (chatWidget && ParentProtocol == dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler()))
			ChatStateMap.insert(chatWidget, new ChatState(chatWidget));
	}

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));
}

void JabberChatStateService::chatWidgetCreated(ChatWidget *chatWidget)
{
	if (ParentProtocol == dynamic_cast<JabberProtocol *>(chatWidget->chat().chatAccount().protocolHandler()))
		ChatStateMap.insert(chatWidget, new ChatState(chatWidget));
}

void JabberChatStateService::chatWidgetDestroying(ChatWidget *chatWidget)
{
	if (ChatStateMap.contains(chatWidget))
	{
		ChatState *state = ChatStateMap.value(chatWidget);
		state->chatClosed();
		ChatStateMap.remove(chatWidget);
		state->deleteLater();
	}
}

ChatState::ChatState(ChatWidget *chatWidget) :
		ObservedChatWidget(chatWidget)
{
	connect(ObservedChatWidget->getChatEditBox()->inputBox(), SIGNAL(textChanged()),
			this, SLOT(setComposing()));

	Protocol = dynamic_cast<JabberProtocol *>(ObservedChatWidget->chat().chatAccount().protocolHandler());
	if (Protocol)
	{
		connect(Protocol->client(), SIGNAL(messageReceived(const XMPP::Message &)),
				this, SLOT(incomingMessage(const XMPP::Message &)));
		connect(Protocol->client(), SIGNAL(messageAboutToSend(XMPP::Message &)),
				this, SLOT(messageAboutToSend(XMPP::Message &)));
	}

	// Message events
	ContactChatState = XMPP::StateNone;
	LastChatState = XMPP::StateNone;
	UserRequestedEvents = false;
	IsComposing = false;
	ComposingTimer = 0;

	connect(this, SIGNAL(composing(bool)), this, SLOT(updateIsComposing(bool)));
}


/**
 * Records that the user is composing
 */
void ChatState::setComposing()
{
	if (!ComposingTimer)
	{
		/* User (re)starts composing */
		ComposingTimer = new QTimer(this);
		connect(ComposingTimer, SIGNAL(timeout()), SLOT(checkComposing()));
		ComposingTimer->start(2000); // FIXME: magic number
		emit composing(true);
	}
	IsComposing = true;
}

/**
 * Checks if the user is still composing
 */
void ChatState::checkComposing()
{
	if (!IsComposing)
	{
		// User stopped composing
		ComposingTimer->deleteLater();
		ComposingTimer = 0;
		emit composing(false);
	}
	IsComposing = false; // Reset composing
}

void ChatState::resetComposing()
{
	if (ComposingTimer)
	{
		delete ComposingTimer;
		ComposingTimer = 0;
	}

	IsComposing = false;
}

void ChatState::updateIsComposing(bool b)
{
	setChatState(b ? XMPP::StateComposing : XMPP::StatePaused);
}

bool ChatState::shouldSendEvent(XMPP::ChatState state)
{
	if (!UserRequestedEvents && ContactChatState != XMPP::StateNone)
		return false;

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(ObservedChatWidget->chat().chatAccount().details());
	if (!jabberAccountDetails)
		return false;

	if (state == XMPP::StateGone && !jabberAccountDetails->sendGoneNotification())
		return false;

	if ((state == XMPP::StateComposing || state == XMPP::StatePaused) && !jabberAccountDetails->sendTypingNotification())
		return false;

	return true;
}

void ChatState::setChatState(XMPP::ChatState state)
{
	if (!shouldSendEvent(state))
		return;

	// Don't send to offline resource
	Contact contact = ObservedChatWidget->chat().contacts().toContact();
	if (contact.currentStatus().isDisconnected())
	{
		UserRequestedEvents = false;
		LastChatState = XMPP::StateNone;
		return;
	}

	//this isn't a valid transition, so don't send it, and don't update laststate
	if (LastChatState == XMPP::StateNone && (state != XMPP::StateActive && state != XMPP::StateComposing && state != XMPP::StateGone))
		return;

	// Check if we should send a message
	if (state == LastChatState || state == XMPP::StateActive || (LastChatState == XMPP::StateActive && state == XMPP::StatePaused))
	{
		LastChatState = state;
		return;
	}

	// Build event message
	XMPP::Message m(ObservedChatWidget->chat().contacts().toContact().id());
	if (UserRequestedEvents)
	{
		m.setEventId(EventId);
		if (state == XMPP::StateComposing)
			m.addEvent(XMPP::ComposingEvent);
		else if (LastChatState == XMPP::StateComposing)
			m.addEvent(XMPP::CancelEvent);
	}

	if (ContactChatState != XMPP::StateNone)
	{
		if (LastChatState != XMPP::StateGone)
		{
			if ((state == XMPP::StateInactive && LastChatState == XMPP::StateComposing)
				|| (state == XMPP::StateComposing && LastChatState == XMPP::StateInactive))
				{
				// First go to the paused state
				XMPP::Message tm(ObservedChatWidget->chat().contacts().toContact().id());
				m.setType("chat");
				m.setChatState(XMPP::StatePaused);

				if (Protocol->isConnected())
					Protocol->client()->client()->sendMessage(m);
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
	if (LastChatState != XMPP::StateGone || state == XMPP::StateActive)
		LastChatState = state;
}

void ChatState::incomingMessage(const XMPP::Message &m)
{
	if (m.from().bare() != (*ObservedChatWidget->chat().contacts().constBegin()).id())
		return;

	if (m.body().isEmpty())
	{
		// Event message
		if (m.containsEvent(XMPP::CancelEvent))
			setContactChatState(XMPP::StatePaused);
		else if (m.containsEvent(XMPP::ComposingEvent))
			setContactChatState(XMPP::StateComposing);

		if (m.chatState() != XMPP::StateNone)
			setContactChatState(m.chatState());
	}
	else
	{
		// Normal message
		// Check if user requests event messages
		UserRequestedEvents = m.containsEvent(XMPP::ComposingEvent);

		if (!m.eventId().isEmpty())
			EventId = m.eventId();

		if (m.containsEvents() || m.chatState() != XMPP::StateNone)
			setContactChatState(XMPP::StateActive);
		else
			setContactChatState(XMPP::StateNone);
	}
}

void ChatState::setContactChatState(XMPP::ChatState state)
{
	ContactChatState = state;
	if (state == XMPP::StateGone)
	{
		Contact contact = ObservedChatWidget->chat().contacts().toContact();
		QString msg = "[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]";
		Message message = Message::create();
		message.setMessageChat(ObservedChatWidget->chat());
		message.setType(Message::TypeSystem);
		message.setMessageSender(contact);
		message.setContent(msg);
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		ObservedChatWidget->chatMessagesView()->appendMessage(message);
	}
	else
	{
		// Activate ourselves
		if (LastChatState == XMPP::StateGone)
			setChatState(XMPP::StateActive);
	}

	updateChatTitle();
}

void ChatState::messageAboutToSend(XMPP::Message &message)
{
	message.addEvent(XMPP::ComposingEvent);
	setChatState(XMPP::StateActive);
}

void ChatState::updateChatTitle()
{
	ObservedChatWidget->refreshTitle();
	QString cap = ObservedChatWidget->title();

	if (ContactChatState == XMPP::StateComposing)
	{
		cap = tr("%1 (Composing ...)").arg(cap);
		ObservedChatWidget->setTitle(cap);
	}
	else if (ContactChatState == XMPP::StateInactive)
	{
		cap = tr("%1 (Inactive)").arg(cap);
		ObservedChatWidget->setTitle(cap);
	}
}

void ChatState::chatClosed()
{
	// Reset 'contact is composing' & cancel own composing event
	resetComposing();
	setChatState(XMPP::StateGone);

	if (ContactChatState == XMPP::StateComposing || ContactChatState == XMPP::StateInactive)
		setContactChatState(XMPP::StatePaused);
}

