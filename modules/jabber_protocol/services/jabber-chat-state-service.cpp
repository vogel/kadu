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

#include "jabber-protocol.h"
#include "jabber-chat-state-service.h"

JabberChatStateService::JabberChatStateService(JabberProtocol *parent) : ParentProtocol(parent)
{
	foreach (Chat chat, ChatManager::instance()->items())
	{
		ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat);
		if (chatWidget && ParentProtocol == dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler()))
			  ChatStateMap.insert(chatWidget, new ChatState(chat));
	}

	QObject::connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
	QObject::connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatWidgetDestroying(ChatWidget *)));
}

void JabberChatStateService::chatWidgetCreated(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (ParentProtocol == dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler()))
		ChatStateMap.insert(chatWidget, new ChatState(chat));
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

ChatState::ChatState(Chat chat) : ObservedChat(chat)
{
	ChatWidget *c = ChatWidgetManager::instance()->byChat(chat);
	if (c)
	{
		Widget = c;
		CustomInput *input = c->getChatEditBox()->inputBox();
		QObject::connect(input, SIGNAL(textChanged()), this, SLOT(setComposing()));
	}

	Protocol = dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler());
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
	SendComposingEvents = false;
	IsComposing = false;
	ComposingTimer = 0;

	QObject::connect(this, SIGNAL(composing(bool)), this, SLOT(updateIsComposing(bool)));
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

void ChatState::setChatState(XMPP::ChatState state)
{
	//TODO 0.6.6
	if (/*PsiOptions::instance()->getOption("options.messages.send-composing-events").toBool() && */(SendComposingEvents || (ContactChatState != XMPP::StateNone)))
	{
		// Don't send to offline resource
		/*QList<UserListItem*> ul = account()->findRelevant(jid());
		if (ul.isEmpty()) {
			sendComposingEvents_ = false;
			lastChatState_ = XMPP::StateNone;
			return;
		}

		UserListItem *u = ul.first();
		if (!u->isAvailable()) {
			sendComposingEvents_ = false;
			lastChatState_ = XMPP::StateNone;
			return;
		}*/

		// Don't send to offline resource
		Contact contact = ObservedChat.contacts().toContact();
		if (contact.currentStatus().isDisconnected())
		{
			SendComposingEvents = false;
			LastChatState = XMPP::StateNone;
			return;
		}

		// Transform to more privacy-enabled chat states if necessary
		if (/*!PsiOptions::instance()->getOption("options.messages.send-inactivity-events").toBool()
			*/false && (state == XMPP::StateGone || state == XMPP::StateInactive))
		{
			state = XMPP::StatePaused;
		}

		if (LastChatState == XMPP::StateNone && (state != XMPP::StateActive && state != XMPP::StateComposing && state != XMPP::StateGone))
		{
			//this isn't a valid transition, so don't send it, and don't update laststate
			return;
		}

		// Check if we should send a message
		if (state == LastChatState || state == XMPP::StateActive || (LastChatState == XMPP::StateActive && state == XMPP::StatePaused))
		{
			LastChatState = state;
			return;
		}

		// Build event message
		XMPP::Message m(ObservedChat.contacts().toContact().id());
		if (SendComposingEvents)
		{
			m.setEventId(EventId);
			if (state == XMPP::StateComposing)
			{
				m.addEvent(XMPP::ComposingEvent);
			}
			else if (LastChatState == XMPP::StateComposing)
			{
				m.addEvent(XMPP::CancelEvent);
			}
		}
		if (ContactChatState != XMPP::StateNone)
		{
			if (LastChatState != XMPP::StateGone)
			{
				if ((state == XMPP::StateInactive && LastChatState == XMPP::StateComposing)
					|| (state == XMPP::StateComposing && LastChatState == XMPP::StateInactive))
				{
					// First go to the paused state
					XMPP::Message tm(ObservedChat.contacts().toContact().id());
					m.setType("chat");
					m.setChatState(XMPP::StatePaused);
					if (Protocol->isConnected())
					{
						Protocol->client()->client()->sendMessage(m);
					}
				}
				m.setChatState(state);
			}
		}

		// Send event message
		if (m.containsEvents() || m.chatState() != XMPP::StateNone)
		{
			m.setType("chat");
			if (Protocol->isConnected())
			{
				Protocol->client()->client()->sendMessage(m);
			}
		}

		// Save last state
		if (LastChatState != XMPP::StateGone || state == XMPP::StateActive)
			LastChatState = state;
	}
}

void ChatState::incomingMessage(const XMPP::Message &m)
{
	if (m.from().bare() != (*ObservedChat.contacts().begin()).id())
		return;

	if (m.body().isEmpty())
	{
		// Event message
		if (m.containsEvent(XMPP::CancelEvent))
		{
			setContactChatState(XMPP::StatePaused);
		}
		else if (m.containsEvent(XMPP::ComposingEvent))
		{
			setContactChatState(XMPP::StateComposing);
		}

		if (m.chatState() != XMPP::StateNone)
		{
			setContactChatState(m.chatState());
		}
	}
	else
	{
		// Normal message
		// Check if user requests event messages
		SendComposingEvents = m.containsEvent(XMPP::ComposingEvent);
		if (!m.eventId().isEmpty())
		{
			EventId = m.eventId();
		}
		if (m.containsEvents() || m.chatState() != XMPP::StateNone)
		{
			setContactChatState(XMPP::StateActive);
		}
		else
		{
			setContactChatState(XMPP::StateNone);
		}
	}
}

void ChatState::setContactChatState(XMPP::ChatState state)
{
	ContactChatState = state;
	if (state == XMPP::StateGone)
	{
		Contact contact = ObservedChat.contacts().toContact();
		QString msg = "[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]";
		Message message = Message::create();
		message.setMessageChat(ObservedChat);
		message.setType(Message::TypeSystem);
		message.setMessageSender(contact);
		message.setContent(msg);
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		Widget->chatMessagesView()->appendMessage(message);
	}
	else
	{
		// Activate ourselves
		if (LastChatState == XMPP::StateGone)
		{
			setChatState(XMPP::StateActive);
		}
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
	ObservedChat.refreshTitle();
	QString cap = ObservedChat.title();

	if (ContactChatState == XMPP::StateComposing)
	{
		cap = tr("%1 (Composing ...)").arg(cap);
		ObservedChat.setTitle(cap);
	}
	else if (ContactChatState == XMPP::StateInactive)
	{
		cap = tr("%1 (Inactive)").arg(cap);
		ObservedChat.setTitle(cap);
	}
	else
	{
		ObservedChat.refreshTitle();
	}
}

void ChatState::chatClosed()
{
	// Reset 'contact is composing' & cancel own composing event
	resetComposing();
	setChatState(XMPP::StateGone);
	if (ContactChatState == XMPP::StateComposing || ContactChatState == XMPP::StateInactive)
	{
		setContactChatState(XMPP::StatePaused);
	}
}

