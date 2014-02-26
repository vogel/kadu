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

#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/chat-style-renderer.h"
#include "core/core.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-display.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/algorithm.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(QObject *parent) :
		QObject{parent}, m_forcePruneDisabled{false}
{
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
}

void HtmlMessagesRenderer::setChatStyleRenderer(qobject_ptr<ChatStyleRenderer> chatStyleRenderer)
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

void HtmlMessagesRenderer::rendererReady()
{
	m_messagesDisplay = make_unique<WebkitMessagesViewDisplay>(*m_chatStyleRenderer.get());
	m_messagesDisplay->setMessageRenderInfoFactory(Core::instance()->messageRenderInfoFactory());
	displayMessages(m_messages);
}

void HtmlMessagesRenderer::displayMessages(const SortedMessages &messages)
{
	if (m_messagesDisplay)
		m_messagesDisplay->displayMessages(messages);
}

void HtmlMessagesRenderer::setForcePruneDisabled(bool forcePruneDisabled)
{
	m_forcePruneDisabled = forcePruneDisabled;
	pruneMessages();
}

bool HtmlMessagesRenderer::isReady() const
{
	return m_chatStyleRenderer && m_chatStyleRenderer->isReady();
}

void HtmlMessagesRenderer::pruneMessages()
{
	if (m_forcePruneDisabled || ChatStyleManager::instance()->cfgNoHeaderRepeat() || m_messages.empty())
		return;

	m_messages = limitMessages(m_messages, ChatStyleManager::instance()->prune());
	displayMessages(m_messages);
}

SortedMessages HtmlMessagesRenderer::limitMessages(const SortedMessages &sortedMessages, int limit)
{
	if (limit <= 0)
		return sortedMessages;

	if (sortedMessages.size() <= static_cast<decltype(sortedMessages.size())>(limit))
		return sortedMessages;

	auto messages = decltype(sortedMessages.messages()){};
	std::copy(end(sortedMessages) - limit, end(sortedMessages), std::back_inserter(messages));
	return SortedMessages{messages};
}

void HtmlMessagesRenderer::add(const Message &message)
{
	m_messages.add(message);
	displayMessages(m_messages);
}

Message HtmlMessagesRenderer::lastMessage() const
{
	return m_messages.empty()
			? Message{}
			: m_messages.last();
}

void HtmlMessagesRenderer::refreshView()
{
	if (m_messagesDisplay)
	{
		displayMessages({});
		displayMessages(m_messages);
	}
}

void HtmlMessagesRenderer::add(const SortedMessages &messages)
{
	if (messages.empty())
		return;

	m_messages.add(messages);

//  Do not prune messages here. When we are adding many massages to renderer, probably
//  we want all of them to be visible on message view. This also fixes crash from
//  bug #1963 . This crash occured, when we are trying to
//  cite more messages from history, than our message pruning setting
//	pruneMessages();

	displayMessages(m_messages);
}

void HtmlMessagesRenderer::clearMessages()
{
	m_messages.clear();
	displayMessages(m_messages);
}

void HtmlMessagesRenderer::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	if (isReady())
		m_chatStyleRenderer->chatImageAvailable(chatImage, fileName);
}

void HtmlMessagesRenderer::messageStatusChanged(const QString &id, MessageStatus status)
{
	if (isReady())
		m_chatStyleRenderer->messageStatusChanged(id, status);
}

void HtmlMessagesRenderer::contactActivityChanged(const Contact &contact, ChatStateService::State state)
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

#include "moc_html-messages-renderer.cpp"