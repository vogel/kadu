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

#include "chat-widget-activation-service.h"

#include "gui/widgets/chat-widget/chat-widget-container-handler.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-mapper.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"

ChatWidgetActivationService::ChatWidgetActivationService(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetActivationService::~ChatWidgetActivationService()
{
}

void ChatWidgetActivationService::setChatWidgetContainerHandlerMapper(ChatWidgetContainerHandlerMapper *chatWidgetContainerHandlerMapper)
{
	m_chatWidgetContainerHandlerMapper = chatWidgetContainerHandlerMapper;
}

void ChatWidgetActivationService::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
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

void ChatWidgetActivationService::chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	connect(chatWidgetContainerHandler, SIGNAL(chatWidgetActivated(ChatWidget*)),
			this, SIGNAL(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetActivationService::chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	disconnect(chatWidgetContainerHandler, SIGNAL(chatWidgetActivated(ChatWidget*)),
			   this, SIGNAL(chatWidgetActivated(ChatWidget*)));
}

bool ChatWidgetActivationService::isChatWidgetActive(ChatWidget *chatWidget) const
{
	if (!m_chatWidgetContainerHandlerMapper || !chatWidget)
		return false;

	auto chatWidgetContainerHandler = m_chatWidgetContainerHandlerMapper.data()->chatWidgetContainerHandlerForWidget(chatWidget);
	return chatWidgetContainerHandler ? chatWidgetContainerHandler->isChatWidgetActive(chatWidget) : false;
}

void ChatWidgetActivationService::tryActivateChatWidget(ChatWidget *chatWidget)
{
	if (!m_chatWidgetContainerHandlerMapper || !chatWidget)
		return;

	auto chatWidgetContainerHandler = m_chatWidgetContainerHandlerMapper.data()->chatWidgetContainerHandlerForWidget(chatWidget);
	if (chatWidgetContainerHandler)
		chatWidgetContainerHandler->tryActivateChatWidget(chatWidget);
}

#include "moc_chat-widget-activation-service.cpp"
