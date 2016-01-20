/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "gui/widgets/chat-widget/chat-widget-message-handler-configuration.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyChatManager;
class Chat;
class ChatWidget;
class ChatWidgetActivationService;
class ChatWidgetManager;
class ChatWidgetRepository;
class Configuration;
class Message;
class MessageManager;
class NotificationService;
class SortedMessages;
class UnreadMessageRepository;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetMessageHandler
 * @short Bridge between chat widgets and messages.
 *
 * This class appends messages received from MessageManager and appends them
 * to proper ChatWidget instances stored in ChatWidgetRepository. It can
 * also request creation of new ChatWidget by ChatWidgetManager and activation
 * by ChatWidgetActivationService if configured to do so.
 *
 * Configuration is passed as ChatWidgetMessageHandlerConfiguration.
 * ChatWidgetMessageHandlerConfigurator can be used to automatically configure
 * it based on configuration file.
 */
class ChatWidgetMessageHandler : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatWidgetMessageHandler(QObject *parent = nullptr);
	virtual ~ChatWidgetMessageHandler();

	void setChatWidgetMessageHandlerConfiguration(ChatWidgetMessageHandlerConfiguration chatWidgetMessageHandlerConfiguration);

private:
	QPointer<BuddyChatManager> m_buddyChatManager;
	QPointer<ChatWidgetActivationService> m_chatWidgetActivationService;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<Configuration> m_configuration;
	QPointer<MessageManager> m_messageManager;
	QPointer<NotificationService> m_notificationService;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	ChatWidgetMessageHandlerConfiguration m_chatWidgetMessageHandlerConfiguration;

	void appendAllUnreadMessages(ChatWidget *chatWidget);
	SortedMessages loadAllUnreadMessages(const Chat &chat) const;
	bool shouldOpenChatWidget(const Chat &chat) const;

private slots:
	INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	INJEQT_SET void setChatWidgetActivationService(ChatWidgetActivationService *chatWidgetActivationService);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setNotificationService(NotificationService *notificationService);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();

	void chatWidgetAdded(ChatWidget *chatWidget);
	void chatWidgetRemoved(ChatWidget *chatWidget);
	void chatWidgetActivated(ChatWidget *chatWidget);

	void messageReceived(const Message &message);
	void messageSent(const Message &message);

};

/**
 * @}
 */
