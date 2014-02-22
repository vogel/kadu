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
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(QObject *parent) :
		QObject{parent}, m_pruneEnabled{true}, m_forcePruneDisabled{false}
{
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
}

void HtmlMessagesRenderer::setChatStyleRenderer(qobject_ptr<ChatStyleRenderer> chatStyleRenderer)
{
	if (m_chatStyleRenderer)
		disconnect(m_chatStyleRenderer.get(), SIGNAL(ready()), this, SLOT(refreshView()));
	m_chatStyleRenderer = std::move(chatStyleRenderer);
	if (m_chatStyleRenderer)
		connect(m_chatStyleRenderer.get(), SIGNAL(ready()), this, SLOT(refreshView()));
}

void HtmlMessagesRenderer::setForcePruneDisabled(bool forcePruneDisabled)
{
	m_forcePruneDisabled = forcePruneDisabled;
	pruneMessages();
}

void HtmlMessagesRenderer::pruneMessages()
{
	if (m_forcePruneDisabled || ChatStyleManager::instance()->cfgNoHeaderRepeat())
		return;

	if (ChatStyleManager::instance()->prune() == 0)
	{
		m_pruneEnabled = false;
		return;
	}

	if (m_messages.count() <= ChatStyleManager::instance()->prune())
	{
		m_pruneEnabled = false;
		return;
	}

	m_pruneEnabled = true;

	auto start = m_messages.begin();
	auto stop = m_messages.end() - ChatStyleManager::instance()->prune();

	if (m_chatStyleRenderer)
		for (auto it = start; it != stop; ++it)
			m_chatStyleRenderer->removeFirstMessage();

	m_messages.erase(start, stop);
}

void HtmlMessagesRenderer::appendMessage(const Message &message)
{
	m_messages.append(message);
	pruneMessages();

	if (m_chatStyleRenderer)
	{
		if (ChatStyleManager::instance()->cfgNoHeaderRepeat() && !m_forcePruneDisabled && m_pruneEnabled)
		{
			refreshView();
			return;
		}
		else
		{
			auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
			auto info = messageRenderInfoFactory->messageRenderInfo(m_lastMessage, message);

			m_chatStyleRenderer->appendChatMessage(message, info);
		}
	}

	m_lastMessage = message;
}

void HtmlMessagesRenderer::refreshView()
{
	if (!m_chatStyleRenderer)
		return;

	m_chatStyleRenderer->clearMessages();

	auto prevMessage = Message::null;
	for (auto const &message : m_messages)
	{
		auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
		auto info = messageRenderInfoFactory->messageRenderInfo(prevMessage, message);
		m_chatStyleRenderer->appendChatMessage(message, info);
		prevMessage = message;
	}
}

void HtmlMessagesRenderer::appendMessages(const QVector<Message> &messages)
{
	if (messages.empty())
		return;

	auto engineMessages = QVector<Message>{};
	for (auto message : messages)
	{
		engineMessages.append(message);
		m_messages.append(message);
	}

//  Do not prune messages here. When we are adding many massages to renderer, probably
//  we want all of them to be visible on message view. This also fixes crash from
//  bug #1963 . This crash occured, when we are trying to
//  cite more messages from history, than our message pruning setting
//	pruneMessages();

	if (m_chatStyleRenderer)
	{
		auto newLastMessage = m_lastMessage;
		for (auto const &message : messages)
		{
			auto info = Core::instance()->messageRenderInfoFactory()->messageRenderInfo(newLastMessage, message);
			m_chatStyleRenderer->appendChatMessage(message, info);
			newLastMessage = message;
		}
	}

	m_lastMessage = messages.last();
}

void HtmlMessagesRenderer::clearMessages()
{
	m_messages.clear();

	m_lastMessage = Message::null;
	if (m_chatStyleRenderer)
		m_chatStyleRenderer->clearMessages();
}

void HtmlMessagesRenderer::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	if (m_chatStyleRenderer)
		m_chatStyleRenderer->chatImageAvailable(chatImage, fileName);
}

void HtmlMessagesRenderer::messageStatusChanged(const QString &id, MessageStatus status)
{
	if (m_chatStyleRenderer)
		m_chatStyleRenderer->messageStatusChanged(id, status);
}

void HtmlMessagesRenderer::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	if (!m_chatStyleRenderer)
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
