/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "webkit-messages-view-handler.h"

#include "chat-style/engine/chat-style-renderer.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display.h"

WebkitMessagesViewHandler::WebkitMessagesViewHandler(qobject_ptr<ChatStyleRenderer> chatStyleRenderer,
	std::unique_ptr<WebkitMessagesViewDisplay> messagesDisplay, QObject *parent) :
		QObject{parent}, m_chatStyleRenderer{std::move(chatStyleRenderer)}, m_messagesDisplay{std::move(messagesDisplay)}
{
	if (m_chatStyleRenderer->isReady())
		rendererReady();
	else
		connect(m_chatStyleRenderer.get(), SIGNAL(ready()), this, SLOT(rendererReady()));
}

WebkitMessagesViewHandler::~WebkitMessagesViewHandler()
{
}

void WebkitMessagesViewHandler::setMessageLimit(unsigned int limit)
{
	m_messagesLimiter.setLimit(limit);
	limitAndDisplayMessages();
}

void WebkitMessagesViewHandler::setMessageLimitPolicy(MessageLimitPolicy messageLimitPolicy)
{
	m_messagesLimiter.setLimitPolicy(messageLimitPolicy);
	limitAndDisplayMessages();
}

void WebkitMessagesViewHandler::rendererReady()
{
	limitAndDisplayMessages();
}

void WebkitMessagesViewHandler::add(const Message &message)
{
	if (!message)
		return;

	m_messages.add(message);
	limitAndDisplayMessages();
}

void WebkitMessagesViewHandler::add(const SortedMessages &messages)
{
	if (messages.empty())
		return;

	m_messages.merge(messages);
	limitAndDisplayMessages();
}

void WebkitMessagesViewHandler::limitAndDisplayMessages()
{
	m_messages = m_messagesLimiter.limitMessages(m_messages);
	if (m_chatStyleRenderer->isReady())
		m_messagesDisplay->displayMessages(m_messages);
}

void WebkitMessagesViewHandler::clear()
{
	m_messages.clear();
	if (m_chatStyleRenderer->isReady())
		m_messagesDisplay->displayMessages(m_messages);
}

void WebkitMessagesViewHandler::displayMessageStatus(const QString &id, MessageStatus status)
{
	if (m_chatStyleRenderer->isReady())
		m_chatStyleRenderer->displayMessageStatus(id, status);
}

void WebkitMessagesViewHandler::displayChatState(const Contact &contact, ChatStateService::State state)
{
	if (!m_chatStyleRenderer->isReady())
		return;

	auto display = contact.display(true);
	auto message = QString{};
	switch (state)
	{
		case ChatStateService::StateActive:
			message = tr("%1 is active").arg(display);
			break;
		case ChatStateService::StateComposing:
			message = tr("%1 is composing...").arg(display);
			break;
		case ChatStateService::StateGone:
			message = tr("%1 is gone").arg(display);
			break;
		case ChatStateService::StateInactive:
			message = tr("%1 is inactive").arg(display);
			break;
		case ChatStateService::StateNone:
			break;
		case ChatStateService::StatePaused:
			message = tr("%1 has paused composing").arg(display);
			break;
	}
	m_chatStyleRenderer->displayChatState(state, message, display);
}

void WebkitMessagesViewHandler::displayChatImage(const ChatImage &chatImage, const QString &fileName)
{
	if (m_chatStyleRenderer->isReady())
		m_chatStyleRenderer->displayChatImage(chatImage, fileName);
}

#include "moc_webkit-messages-view-handler.cpp"
