/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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
		if (ParentProtocol == dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler()))
		{
			ChatWidget *c = ChatWidgetManager::instance()->byChat(chat);
			if (c)
			{
			  	ChatStateList.append(new ChatState(chat));
			}
		}
	}
	
	QObject::connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatWidgetCreated(ChatWidget *)));
}

void JabberChatStateService::chatWidgetCreated(ChatWidget *chatWidget)
{
	Chat chat = chatWidget->chat();
	if (ParentProtocol == dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler()))
	{
		ChatWidget *c = ChatWidgetManager::instance()->byChat(chat);
		if (c)
		{
			ChatStateList.append(new ChatState(chat));
		}
	}
}


ChatState::ChatState(Chat chat) : ObservedChat(chat)
{
	ChatWidget *c = ChatWidgetManager::instance()->byChat(chat);
	if (c)
	{
		CustomInput *input = c->getChatEditBox()->inputBox();
		QObject::connect(input, SIGNAL(textChanged()), this, SLOT(setComposing()));
	}
	
	JabberProtocol *p = dynamic_cast<JabberProtocol *>(chat.chatAccount().protocolHandler());
	if (p)
	{
		connect(p->client(), SIGNAL(messageReceived(const XMPP::Message &)),
			this, SLOT(incomingMessage(const XMPP::Message &)));
		connect(p->client(), SIGNAL(messageAboutToSend(XMPP::Message &)),
			this, SLOT(messageAboutToSend(XMPP::Message &))); 
	}
	Protocol = p;
	
	// Message events
	contactChatState_ = XMPP::StateNone;
	lastChatState_ = XMPP::StateNone;
	sendComposingEvents_ = false;
	isComposing_ = false;
	composingTimer_ = 0;
	titleChanged = false;
		
	
	QObject::connect(this, SIGNAL(composing(bool)), this, SLOT(updateIsComposing(bool)));

}
			

/**
 * Records that the user is composing
 */
void ChatState::setComposing()
{
	if (!composingTimer_) {
		/* User (re)starts composing */
		composingTimer_ = new QTimer(this);
		connect(composingTimer_, SIGNAL(timeout()), SLOT(checkComposing()));
		composingTimer_->start(2000); // FIXME: magic number
		emit composing(true);
	}
	isComposing_ = true;
}

/**
 * Checks if the user is still composing
 */
void ChatState::checkComposing()
{
	if (!isComposing_) {
		// User stopped composing
		composingTimer_->deleteLater();
		composingTimer_ = 0;
		emit composing(false);
	}
	isComposing_ = false; // Reset composing
}

void ChatState::resetComposing()
{
	if (composingTimer_) {
		delete composingTimer_;
		composingTimer_ = 0;
		isComposing_ = false;
	}
}

void ChatState::updateIsComposing(bool b)
{
	setChatState(b ? XMPP::StateComposing : XMPP::StatePaused);
}

void ChatState::setChatState(XMPP::ChatState state)
{
	if (/*PsiOptions::instance()->getOption("options.messages.send-composing-events").toBool() && */(sendComposingEvents_ || (contactChatState_ != XMPP::StateNone)))
	{
		// Don't send to offline resource
// 		QList<UserListItem*> ul = account()->findRelevant(jid());
// 		if (ul.isEmpty()) {
// 			sendComposingEvents_ = false;
// 			lastChatState_ = XMPP::StateNone;
// 			return;
// 		}
// 
// 		UserListItem *u = ul.first();
// 		if (!u->isAvailable()) {
// 			sendComposingEvents_ = false;
// 			lastChatState_ = XMPP::StateNone;
// 			return;
// 		}

		// Transform to more privacy-enabled chat states if necessary
		if (/*!PsiOptions::instance()->getOption("options.messages.send-inactivity-events").toBool() && */(state == XMPP::StateGone || state == XMPP::StateInactive)) {
			state = XMPP::StatePaused;
		}

		if (lastChatState_ == XMPP::StateNone && (state != XMPP::StateActive && state != XMPP::StateComposing && state != XMPP::StateGone)) {
			//this isn't a valid transition, so don't send it, and don't update laststate
			return;
		}

		// Check if we should send a message
		if (state == lastChatState_ || state == XMPP::StateActive || (lastChatState_ == XMPP::StateActive && state == XMPP::StatePaused)) {
			lastChatState_ = state;
			return;
		}

		// Build event message
		XMPP::Message m(ObservedChat.contacts().toContact().id());
		if (sendComposingEvents_) {
			m.setEventId(eventId_);
			if (state == XMPP::StateComposing) {
				m.addEvent(XMPP::ComposingEvent);
			}
			else if (lastChatState_ == XMPP::StateComposing) {
				m.addEvent(XMPP::CancelEvent);
			}
		}
		if (contactChatState_ != XMPP::StateNone) {
			if (lastChatState_ != XMPP::StateGone) {
				if ((state == XMPP::StateInactive && lastChatState_ == XMPP::StateComposing) || (state == XMPP::StateComposing && lastChatState_ == XMPP::StateInactive)) {
					// First go to the paused state
					XMPP::Message tm(ObservedChat.contacts().toContact().id());
					m.setType("chat");
					m.setChatState(XMPP::StatePaused);
					if (Protocol->isConnected()) {
						Protocol->client()->client()->sendMessage(m);
					}
				}
				m.setChatState(state);
			}
		}

		// Send event message
		if (m.containsEvents() || m.chatState() != XMPP::StateNone) {
			m.setType("chat");
			if (Protocol->isConnected()) {
				Protocol->client()->client()->sendMessage(m);
			}
		}

		// Save last state
		if (lastChatState_ != XMPP::StateGone || state == XMPP::StateActive)
			lastChatState_ = state;
	}
}

void ChatState::incomingMessage(const XMPP::Message &m)
{
	if (m.body().isEmpty()) {
		// Event message
		if (m.containsEvent(XMPP::CancelEvent)) {
			setContactChatState(XMPP::StatePaused);
		}
		else if (m.containsEvent(XMPP::ComposingEvent)) {
			setContactChatState(XMPP::StateComposing);
		}

		if (m.chatState() != XMPP::StateNone) {
			setContactChatState(m.chatState());
		}
	}
	else {
		// Normal message
		// Check if user requests event messages
		sendComposingEvents_ = m.containsEvent(XMPP::ComposingEvent);
		if (!m.eventId().isEmpty()) {
			eventId_ = m.eventId();
		}
		if (m.containsEvents() || m.chatState() != XMPP::StateNone) {
			setContactChatState(XMPP::StateActive);
		}
		else {
			setContactChatState(XMPP::StateNone);
		}
	}
}

void ChatState::setContactChatState(XMPP::ChatState state)
{
	contactChatState_ = state;
	if (state == XMPP::StateGone) {
		///appendSysMsg(tr("%1 ended the conversation").arg(Qt::escape(dispNick_)));
	}
	else {
		// Activate ourselves
		if (lastChatState_ == XMPP::StateGone) {
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
	QString cap = ObservedChat.title();

	if (titleChanged)
	{
		titleChanged = false;
		ObservedChat.refreshTitle();
	}
	else if (contactChatState_ == XMPP::StateComposing && !titleChanged)
	{
		cap = tr("%1 (Composing ...)").arg(cap);
		ObservedChat.setTitle(cap);
		titleChanged = true;
	}
	else if (contactChatState_ == XMPP::StateInactive && !titleChanged)
	{
		cap = tr("%1 (Inactive)").arg(cap);
		ObservedChat.setTitle(cap);
		titleChanged = true;
	}
}