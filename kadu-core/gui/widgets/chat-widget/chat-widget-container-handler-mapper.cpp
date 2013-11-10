/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-container-handler-mapper.h"

#include "gui/widgets/chat-widget/chat-widget-container-handler-mapper.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"

ChatWidgetContainerHandlerMapper::ChatWidgetContainerHandlerMapper(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetContainerHandlerMapper::~ChatWidgetContainerHandlerMapper()
{
}

void ChatWidgetContainerHandlerMapper::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;

	if (!m_chatWidgetContainerHandlerRepository)
		return;

	connect(m_chatWidgetContainerHandlerRepository.data(), SIGNAL(chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler*)),
			this, SLOT(chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler*)));
	connect(m_chatWidgetContainerHandlerRepository.data(), SIGNAL(chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler*)),
			this, SLOT(chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler*)));
}

void ChatWidgetContainerHandlerMapper::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (!m_chatWidgetRepository)
		return;

	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget*)),
			this, SLOT(chatWidgetAdded(ChatWidget*)));
	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)),
			this, SLOT(chatWidgetRemoved(ChatWidget*)));
}

void ChatWidgetContainerHandlerMapper::mapChatWidgetToFirstContainerHandler(ChatWidget *chatWidget)
{
	if (!m_chatWidgetContainerHandlerRepository)
		return;

	auto chatWidgetContainerHandlers = m_chatWidgetContainerHandlerRepository.data()->chatWidgetContainerHandlers();
	for (auto chatWidgetContainerHandler : chatWidgetContainerHandlers)
		if (chatWidgetContainerHandler->wantChatWidget(chatWidget))
		{
			chatWidgetContainerHandler->addChatWidget(chatWidget);
			m_mapping.insert(chatWidget, chatWidgetContainerHandler);
			return;
		}
}

bool ChatWidgetContainerHandlerMapper::isChatWidgetActive(ChatWidget *chatWidget) const
{
	if (!chatWidget)
		return false;

	auto chatWidgetContainerHandler = m_mapping.value(chatWidget);
	return chatWidgetContainerHandler ? chatWidgetContainerHandler->isChatWidgetActive(chatWidget) : false;
}

void ChatWidgetContainerHandlerMapper::tryActivateChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	auto chatWidgetContainerHandler = m_mapping.value(chatWidget);
	if (chatWidgetContainerHandler)
		chatWidgetContainerHandler->tryActivateChatWidget(chatWidget);
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	auto chatWidgets = m_mapping.keys();
	for (auto chatWidget : chatWidgets)
	{
		if (chatWidgetContainerHandler->wantChatWidget(chatWidget))
		{
			auto oldChatWidgetContainerHandler = m_mapping.value(chatWidget);
			oldChatWidgetContainerHandler->removeChatWidget(chatWidget);
			chatWidgetContainerHandler->addChatWidget(chatWidget);
		}
	}
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	auto chatWidgets = m_mapping.keys(chatWidgetContainerHandler);
	for (auto chatWidget : chatWidgets)
	{
		m_mapping.remove(chatWidget);
		mapChatWidgetToFirstContainerHandler(chatWidget);
	}
}

void ChatWidgetContainerHandlerMapper::chatWidgetAdded(ChatWidget *chatWidget)
{
	mapChatWidgetToFirstContainerHandler(chatWidget);
}

void ChatWidgetContainerHandlerMapper::chatWidgetRemoved(ChatWidget *chatWidget)
{
	auto chatWidgetContainerHandler = m_mapping.value(chatWidget);
	if (chatWidgetContainerHandler)
		chatWidgetContainerHandler->removeChatWidget(chatWidget);

	m_mapping.remove(chatWidget);
}
