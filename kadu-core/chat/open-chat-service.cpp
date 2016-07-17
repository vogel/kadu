/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-chat-service.h"

#include "chat/open-chat-repository.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

OpenChatService::OpenChatService(QObject *parent) :
		QObject{parent}
{
}

OpenChatService::~OpenChatService()
{
}

void OpenChatService::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void OpenChatService::setOpenChatRepository(OpenChatRepository *openChatRepository)
{
	m_openChatRepository = openChatRepository;
}

void OpenChatService::init()
{
	connect(m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetAdded, this, &OpenChatService::chatWidgetAdded);
	connect(m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetRemoved, this, &OpenChatService::chatWidgetRemoved);

	for (auto chatWidget : m_chatWidgetRepository)
		chatWidgetAdded(chatWidget);
}

void OpenChatService::chatWidgetAdded(ChatWidget *chatWidget)
{
	m_openChatRepository->addOpenChat(chatWidget->chat());
}

void OpenChatService::chatWidgetRemoved(ChatWidget *chatWidget)
{
	m_openChatRepository->removeOpenChat(chatWidget->chat());
}

#include "open-chat-service.moc"
