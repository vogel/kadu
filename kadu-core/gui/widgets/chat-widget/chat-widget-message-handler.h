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

#pragma once

#include "gui/widgets/chat-widget/chat-widget-message-handler-configuration.h"

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class BuddyChatManager;
class Chat;
class ChatWidget;
class ChatWidgetContainerHandlerMapper;
class ChatWidgetManager;
class ChatWidgetRepository;
class Message;
class MessageManager;
class NotificationService;
class UnreadMessageRepository;

class ChatWidgetMessageHandler : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetMessageHandler(QObject *parent = nullptr);
	virtual ~ChatWidgetMessageHandler();

	void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	void setChatWidgetContainerHandlerMapper(ChatWidgetContainerHandlerMapper *chatWidgetContainerHandlerMapper);
	void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	void setMessageManager(MessageManager *messageManager);
	void setNotificationService(NotificationService *notificationService);
	void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);

	void setConfiguration(ChatWidgetMessageHandlerConfiguration configuration);

private:
	QWeakPointer<BuddyChatManager> m_buddyChatManager;
	QWeakPointer<ChatWidgetContainerHandlerMapper> m_chatWidgetContainerHandlerMapper;
	QWeakPointer<ChatWidgetManager> m_chatWidgetManager;
	QWeakPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QWeakPointer<MessageManager> m_messageManager;
	QWeakPointer<NotificationService> m_notificationService;
	QWeakPointer<UnreadMessageRepository> m_unreadMessageRepository;

	ChatWidgetMessageHandlerConfiguration m_configuration;

	QVector<Message> loadAllUnreadMessages(const Chat &chat) const;
	bool shouldOpenChatWidget(const Chat &chat) const;

private slots:
	void chatWidgetAdded(ChatWidget *chatWidget);
	void chatWidgetRemoved(ChatWidget *chatWidget);
	void chatWidgetActivated(ChatWidget *chatWidget);

	void messageReceived(const Message &message);
	void messageSent(const Message &message);

};
