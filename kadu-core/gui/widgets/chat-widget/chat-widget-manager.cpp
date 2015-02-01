/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat-widget-manager.h"

#include "gui/widgets/chat-widget/chat-widget-activation-service.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-mapper.h"
#include "gui/widgets/chat-widget/chat-widget-factory.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

ChatWidgetManager::ChatWidgetManager(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetManager::~ChatWidgetManager()
{
}

void ChatWidgetManager::setChatWidgetActivationService(ChatWidgetActivationService *chatWidgetActivationService)
{
	m_chatWidgetActivationService = chatWidgetActivationService;
}

void ChatWidgetManager::setChatWidgetContainerHandlerMapper(ChatWidgetContainerHandlerMapper *chatWidgetContainerHandlerMapper)
{
	m_chatWidgetContainerHandlerMapper = chatWidgetContainerHandlerMapper;
}

void ChatWidgetManager::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void ChatWidgetManager::setChatWidgetFactory(ChatWidgetFactory *chatWidgetFactory)
{
	m_chatWidgetFactory = chatWidgetFactory;
}

ChatWidget * ChatWidgetManager::openChat(const Chat &chat, OpenChatActivation activation)
{
	if (!chat || !m_chatWidgetFactory || !m_chatWidgetRepository)
		return nullptr;

	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	if (!chatWidget)
	{
		chatWidget = m_chatWidgetContainerHandlerMapper->createHandledChatWidget(chat, activation);
		if (!chatWidget)
			return nullptr;
	}

	if (m_chatWidgetActivationService)
		switch (activation)
		{
			case OpenChatActivation::Activate:
				m_chatWidgetActivationService.data()->tryActivateChatWidget(chatWidget);
				break;
			case OpenChatActivation::Minimize:
				m_chatWidgetActivationService.data()->tryMinimizeChatWidget(chatWidget);
				break;
			default:
				break;
		}

	return chatWidget;
}

#include "moc_chat-widget-manager.cpp"
