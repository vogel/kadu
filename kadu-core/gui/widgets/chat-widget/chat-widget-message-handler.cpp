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

#include "chat-widget-message-handler.h"

#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "message/unread-message-repository.h"

ChatWidgetMessageHandler::ChatWidgetMessageHandler(QObject *parent) :
		QObject(parent)
{
}

ChatWidgetMessageHandler::~ChatWidgetMessageHandler()
{
}

void ChatWidgetMessageHandler::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (!m_chatWidgetRepository)
		return;

	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(chatWidgetCreated(ChatWidget*)));
	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetDestroyed(ChatWidget*)), this, SLOT(chatWidgetDestroyed(ChatWidget*)));

	foreach (auto chatWidget, m_chatWidgetRepository.data()->widgets())
		chatWidgetCreated(chatWidget);
}

void ChatWidgetMessageHandler::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

void ChatWidgetMessageHandler::chatWidgetCreated(ChatWidget *chatWidget)
{
	connect(chatWidget, SIGNAL(activated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetMessageHandler::chatWidgetDestroyed(ChatWidget *chatWidget)
{
	disconnect(chatWidget, SIGNAL(activated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetMessageHandler::chatWidgetActivated(ChatWidget *chatWidget)
{
	if (!m_unreadMessageRepository)
		return;

	auto chat = chatWidget->chat();
	auto messages = m_unreadMessageRepository.data()->unreadMessagesForChat(chat);
	m_unreadMessageRepository.data()->markMessagesAsRead(messages);
}
