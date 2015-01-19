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
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

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
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget*)), this, SLOT(chatWidgetRemoved(ChatWidget*)));
}

ChatWidgetContainerHandler * ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerForChat(Chat chat) const
{
	if (!chat)
		return {};
	return m_mapping.value(chat);
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	connect(chatWidgetContainerHandler, SIGNAL(chatAcceptanceChanged(Chat)),
			this, SLOT(chatAcceptanceChanged(Chat)));

	auto chats = m_mapping.keys();
	for (auto chat : chats)
		if (chatWidgetContainerHandler->acceptChat(chat))
		{
			unmap(chat);
			map(chatWidgetContainerHandler, chat, OpenChatActivation::Ignore);
		}
}

void ChatWidgetContainerHandlerMapper::chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	disconnect(chatWidgetContainerHandler, SIGNAL(chatAcceptanceChanged(Chat)),
			   this, SLOT(chatAcceptanceChanged(Chat)));

	auto chatWidgets = m_mapping.keys(chatWidgetContainerHandler);
	for (auto chatWidget : chatWidgets)
	{
		unmap(chatWidget);
		mapToDefault(chatWidget, OpenChatActivation::Ignore);
	}
}

void ChatWidgetContainerHandlerMapper::chatAcceptanceChanged(Chat chat)
{
	unmap(chat);
	mapToDefault(chat, OpenChatActivation::Ignore);
}

ChatWidget * ChatWidgetContainerHandlerMapper::mapToDefault(Chat chat, OpenChatActivation activation)
{
	return map(bestContainerHandler(chat), chat, activation);
}

ChatWidget * ChatWidgetContainerHandlerMapper::map(ChatWidgetContainerHandler *chatWidgetContainerHandler, Chat chat, OpenChatActivation activation)
{
	if (!chatWidgetContainerHandler)
		return nullptr;

	m_mapping.insert(chat, chatWidgetContainerHandler);

	auto chatWidget = chatWidgetContainerHandler->addChat(chat, activation);
	if (m_chatWidgetRepository)
		m_chatWidgetRepository.data()->addChatWidget(chatWidget);

	return chatWidget;
}

void ChatWidgetContainerHandlerMapper::unmap(Chat chat)
{
	auto chatWidgetContainerHandler = m_mapping.value(chat);
	m_mapping.remove(chat);
	if (!chatWidgetContainerHandler)
		return;

	chatWidgetContainerHandler->removeChat(chat);
	if (m_chatWidgetRepository)
		m_chatWidgetRepository->removeChatWidget(chat);
}

ChatWidgetContainerHandler * ChatWidgetContainerHandlerMapper::bestContainerHandler(Chat chat) const
{
	if (!m_chatWidgetContainerHandlerRepository || !chat)
		return {};

	auto chatWidgetContainerHandlers = m_chatWidgetContainerHandlerRepository.data()->chatWidgetContainerHandlers();
	auto accepted = [chat](ChatWidgetContainerHandler *chatWidgetContainerHandler){ return chatWidgetContainerHandler->acceptChat(chat); };
	auto result = std::find_if(chatWidgetContainerHandlers.begin(), chatWidgetContainerHandlers.end(), accepted);
	return result == chatWidgetContainerHandlers.end() ? nullptr : *result;
}

void ChatWidgetContainerHandlerMapper::chatWidgetRemoved(ChatWidget *chatWidget)
{
	if (chatWidget)
		unmap(chatWidget->chat());
}

#include "moc_chat-widget-container-handler-mapper.cpp"
