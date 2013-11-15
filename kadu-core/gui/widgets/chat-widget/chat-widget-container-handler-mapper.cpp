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

#include <algorithm>

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

	for (auto chatWidgetContainerHandler : m_chatWidgetContainerHandlerRepository.data())
		chatWidgetContainerHandlerRegistered(chatWidgetContainerHandler);
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

	for (auto chatWidget : m_chatWidgetRepository.data())
		chatWidgetAdded(chatWidget);
}

ChatWidgetContainerHandler * ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerForWidget(ChatWidget *chatWidget) const
{
	if (!chatWidget)
		return {};
	return m_mapping.value(chatWidget);
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	connect(chatWidgetContainerHandler, SIGNAL(chatWidgetAcceptanceChanged(ChatWidget*)),
			this, SLOT(chatWidgetAcceptanceChanged(ChatWidget*)));

	auto chatWidgets = m_mapping.keys();
	for (auto chatWidget : chatWidgets)
		if (chatWidgetContainerHandler->acceptChatWidget(chatWidget))
		{
			unmap(chatWidget);
			map(chatWidgetContainerHandler, chatWidget);
		}
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	disconnect(chatWidgetContainerHandler, SIGNAL(chatWidgetAcceptanceChanged(ChatWidget*)),
			   this, SLOT(chatWidgetAcceptanceChanged(ChatWidget*)));

	auto chatWidgets = m_mapping.keys(chatWidgetContainerHandler);
	for (auto chatWidget : chatWidgets)
	{
		unmap(chatWidget);
		mapToDefault(chatWidget);
	}
}

void ChatWidgetContainerHandlerMapper::chatWidgetAcceptanceChanged(ChatWidget *chatWidget)
{
	unmap(chatWidget);
	mapToDefault(chatWidget);
}

void ChatWidgetContainerHandlerMapper::mapToDefault(ChatWidget *chatWidget)
{
	map(bestContainerHandler(chatWidget), chatWidget);
}

void ChatWidgetContainerHandlerMapper::map(ChatWidgetContainerHandler *chatWidgetContainerHandler, ChatWidget *chatWidget)
{
	if (!chatWidgetContainerHandler)
		return;

	chatWidgetContainerHandler->addChatWidget(chatWidget);
	m_mapping.insert(chatWidget, chatWidgetContainerHandler);
}

void ChatWidgetContainerHandlerMapper::unmap(ChatWidget *chatWidget)
{
	auto chatWidgetContainerHandler = m_mapping.value(chatWidget);
	if (chatWidgetContainerHandler)
		chatWidgetContainerHandler->removeChatWidget(chatWidget);
}

ChatWidgetContainerHandler * ChatWidgetContainerHandlerMapper::bestContainerHandler(ChatWidget *chatWidget) const
{
	if (!m_chatWidgetContainerHandlerRepository)
		return {};

	auto chatWidgetContainerHandlers = m_chatWidgetContainerHandlerRepository.data()->chatWidgetContainerHandlers();
	auto accepted = [chatWidget](ChatWidgetContainerHandler *chatWidgetContainerHandler){ return chatWidgetContainerHandler->acceptChatWidget(chatWidget); };
	auto result = std::find_if(chatWidgetContainerHandlers.begin(), chatWidgetContainerHandlers.end(), accepted);
	return result == chatWidgetContainerHandlers.end() ? nullptr : *result;
}

void ChatWidgetContainerHandlerMapper::chatWidgetAdded(ChatWidget *chatWidget)
{
	mapToDefault(chatWidget);
}

void ChatWidgetContainerHandlerMapper::chatWidgetRemoved(ChatWidget *chatWidget)
{
	auto chatWidgetContainerHandler = m_mapping.value(chatWidget);
	if (chatWidgetContainerHandler)
		chatWidgetContainerHandler->removeChatWidget(chatWidget);

	m_mapping.remove(chatWidget);
}

#include "moc_chat-widget-container-handler-mapper.cpp"
