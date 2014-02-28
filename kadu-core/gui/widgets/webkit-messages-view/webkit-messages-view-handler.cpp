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
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display-factory.h"

WebkitMessagesViewHandler::WebkitMessagesViewHandler(QObject *parent) :
		QObject{parent}
{
}

WebkitMessagesViewHandler::~WebkitMessagesViewHandler()
{
}

void WebkitMessagesViewHandler::setWebkitMessagesViewDisplayFactory(WebkitMessagesViewDisplayFactory *webkitMessagesViewDisplayFactory)
{
	m_webkitMessagesViewDisplayFactory = webkitMessagesViewDisplayFactory;
}

void WebkitMessagesViewHandler::setChatStyleRenderer(qobject_ptr<ChatStyleRenderer> chatStyleRenderer)
{
	if (m_chatStyleRenderer)
		disconnect(m_chatStyleRenderer.get(), SIGNAL(ready()), this, SLOT(rendererReady()));
	m_chatStyleRenderer = std::move(chatStyleRenderer);

	m_messagesDisplay.reset();
	if (!m_chatStyleRenderer)
		return;

	if (m_chatStyleRenderer->isReady())
		rendererReady();
	else
		connect(m_chatStyleRenderer.get(), SIGNAL(ready()), this, SLOT(rendererReady()));
}

void WebkitMessagesViewHandler::setMessageLimit(unsigned int limit)
{
	m_messagesLimiter.setLimit(limit);
	m_messages = m_messagesLimiter.limitMessages(m_messages);
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::setMessageLimitPolicy(MessageLimitPolicy messageLimitPolicy)
{
	m_messagesLimiter.setLimitPolicy(messageLimitPolicy);
	m_messages = m_messagesLimiter.limitMessages(m_messages);
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::rendererReady()
{
	if (m_webkitMessagesViewDisplayFactory)
		m_messagesDisplay = m_webkitMessagesViewDisplayFactory->createWebkitMessagesViewDisplay(*m_chatStyleRenderer.get());
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::displayMessages(const SortedMessages &messages)
{
	if (m_messagesDisplay)
		m_messagesDisplay->displayMessages(messages);
}

bool WebkitMessagesViewHandler::isReady() const
{
	return m_chatStyleRenderer && m_chatStyleRenderer->isReady();
}

SortedMessages WebkitMessagesViewHandler::limitMessages(SortedMessages messages) const
{
	return m_messagesLimiter.limitMessages(messages);
}

void WebkitMessagesViewHandler::add(const Message &message)
{
	m_messages.add(message);
	m_messages = limitMessages(m_messages);
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::refreshView()
{
	if (m_messagesDisplay)
	{
		displayMessages({});
		displayMessages(m_messages);
	}
}

void WebkitMessagesViewHandler::add(const SortedMessages &messages)
{
	if (messages.empty())
		return;

	m_messages.add(messages);
	m_messages = limitMessages(m_messages);
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::clear()
{
	m_messages.clear();
	displayMessages(m_messages);
}

void WebkitMessagesViewHandler::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	if (isReady())
		m_chatStyleRenderer->chatImageAvailable(chatImage, fileName);
}

void WebkitMessagesViewHandler::messageStatusChanged(const QString &id, MessageStatus status)
{
	if (isReady())
		m_chatStyleRenderer->messageStatusChanged(id, status);
}

void WebkitMessagesViewHandler::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	if (!isReady())
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
	m_chatStyleRenderer->contactActivityChanged(state, message, display);
}

#include "moc_webkit-messages-view-handler.cpp"
