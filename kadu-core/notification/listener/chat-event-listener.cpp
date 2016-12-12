/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "widgets/chat-widget/chat-widget-repository.h"
#include "widgets/chat-widget/chat-widget.h"
#include "message/message-manager.h"
#include "message/message-notification-service.h"
#include "message/message.h"
#include "notification/notification-configuration.h"
#include "activate.h"

#include "chat-event-listener.h"

ChatEventListener::ChatEventListener(QObject *parent) :
		QObject{parent}
{
}

ChatEventListener::~ChatEventListener()
{
}

void ChatEventListener::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void ChatEventListener::setMessageManager(MessageManager *messageManager)
{
	connect(messageManager, SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
}

void ChatEventListener::setMessageNotificationService(MessageNotificationService *messageNotificationService)
{
	m_messageNotificationService = messageNotificationService;
}

void ChatEventListener::setNotificationConfiguration(NotificationConfiguration *notificationConfiguration)
{
	m_notificationConfiguration = notificationConfiguration;
}

void ChatEventListener::messageReceived(const Message &message)
{
	if (message.messageChat().isOpen())
	{
		auto chatWidget = m_chatWidgetRepository->widgetForChat(message.messageChat());
		if (!m_notificationConfiguration->newMessageOnlyIfInactive() || !_isWindowActiveOrFullyVisible(chatWidget))
			m_messageNotificationService->notifyNewMessage(message);
	}
	else
		m_messageNotificationService->notifyNewChat(message);
}

#include "moc_chat-event-listener.cpp"
