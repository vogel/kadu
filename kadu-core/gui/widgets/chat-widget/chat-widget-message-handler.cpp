/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/buddy-chat-manager.h"
#include "configuration/configuration-api.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-activation-service.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/kadu-window.h"
#include "message/message-manager.h"
#include "message/sorted-messages.h"
#include "message/unread-message-repository.h"
#include "notification/notification-service.h"
#include "protocols/protocol.h"

#include <QtWidgets/QApplication>

ChatWidgetMessageHandler::ChatWidgetMessageHandler(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetMessageHandler::~ChatWidgetMessageHandler()
{
}

void ChatWidgetMessageHandler::setChatWidgetActivationService(ChatWidgetActivationService *chatWidgetActivationService)
{
	m_chatWidgetActivationService = chatWidgetActivationService;

	if (!m_chatWidgetActivationService)
		return;

	connect(m_chatWidgetActivationService.data(), SIGNAL(chatWidgetActivated(ChatWidget*)),
			this, SLOT(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetMessageHandler::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void ChatWidgetMessageHandler::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (!m_chatWidgetRepository)
		return;

	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget*)), this, SLOT(chatWidgetAdded(ChatWidget*)));
	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)), this, SLOT(chatWidgetRemoved(ChatWidget*)));

	for (auto chatWidget : m_chatWidgetRepository.data())
		chatWidgetAdded(chatWidget);
}

void ChatWidgetMessageHandler::setMessageManager(MessageManager *messageManager)
{
	m_messageManager = messageManager;

	if (!m_messageManager)
		return;

	// some other messageReceived slot may check if message chat is open and this
	// slot can change this value, so let all other messageReceived be executed before this
	connect(m_messageManager.data(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)), Qt::QueuedConnection);
	connect(m_messageManager.data(), SIGNAL(messageSent(Message)), this, SLOT(messageSent(Message)), Qt::QueuedConnection);
}

void ChatWidgetMessageHandler::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void ChatWidgetMessageHandler::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;

	if (!m_unreadMessageRepository)
		return;
}

void ChatWidgetMessageHandler::setConfiguration(ChatWidgetMessageHandlerConfiguration configuration)
{
	m_configuration = configuration;
}

void ChatWidgetMessageHandler::chatWidgetAdded(ChatWidget *chatWidget)
{
	appendAllUnreadMessages(chatWidget);
}

void ChatWidgetMessageHandler::chatWidgetRemoved(ChatWidget *chatWidget)
{
	auto chat = chatWidget->chat();
	chat.removeProperty("message:unreadMessagesAppended");
}

void ChatWidgetMessageHandler::chatWidgetActivated(ChatWidget *chatWidget)
{
	appendAllUnreadMessages(chatWidget);
}

void ChatWidgetMessageHandler::appendAllUnreadMessages(ChatWidget *chatWidget)
{
	if (!m_unreadMessageRepository)
		return;

	auto chat = chatWidget->chat();
	auto unreadMessagesAppended = chat.property("message:unreadMessagesAppended", false).toBool();

	auto messages = unreadMessagesAppended ? m_unreadMessageRepository.data()->unreadMessagesForChat(chat) : loadAllUnreadMessages(chat);
	auto chatIsActive = m_chatWidgetActivationService ? m_chatWidgetActivationService.data()->isChatWidgetActive(chatWidget) : false;
	if (chatIsActive)
		m_unreadMessageRepository.data()->markMessagesAsRead(messages);

	if (!unreadMessagesAppended)
	{
		chatWidget->addMessages(messages);
		chat.addProperty("message:unreadMessagesAppended", true, CustomProperties::NonStorable);
	}
}

SortedMessages ChatWidgetMessageHandler::loadAllUnreadMessages(const Chat &chat) const
{
	// TODO: BuddyChatManager cannot be injected here, because it crashes, find out why
	auto buddyChat = BuddyChatManager::instance()->buddyChat(chat);
	auto unreadChat = buddyChat ? buddyChat : chat;
	return m_unreadMessageRepository.data()->unreadMessagesForChat(unreadChat);
}

void ChatWidgetMessageHandler::messageReceived(const Message &message)
{
	if (!m_chatWidgetRepository)
		return;

	auto chat = message.messageChat();
	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	auto chatIsActive = m_chatWidgetActivationService ? m_chatWidgetActivationService.data()->isChatWidgetActive(chatWidget) : false;

	if (m_unreadMessageRepository && !chatIsActive)
		m_unreadMessageRepository.data()->addUnreadMessage(message);

	if (chatWidget)
	{
		chatWidget->addMessage(message);
		return;
	}

	if (shouldOpenChatWidget(chat))
	{
		auto activation = m_configuration.openChatOnMessageMinimized()
			? OpenChatActivation::Minimize
			: OpenChatActivation::Activate;
		m_chatWidgetManager.data()->openChat(chat, activation);
	}
	else
	{
#ifdef Q_OS_WIN
		if (!Core::instance()->application()->configuration()->deprecatedApi()->readBoolEntry("General", "HideMainWindowFromTaskbar"))
			qApp->alert(Core::instance()->kaduWindow());
#else
		qApp->alert(Core::instance()->kaduWindow());
#endif
	}
}

bool ChatWidgetMessageHandler::shouldOpenChatWidget(const Chat &chat) const
{
	if (!m_configuration.openChatOnMessage())
		return false;

	auto silentMode = m_notificationService ? m_notificationService.data()->silentMode() : false;
	if (silentMode)
		return false;

	auto handler = chat.chatAccount().protocolHandler();
	if (!handler)
		return false;

	if (m_configuration.openChatOnMessageOnlyWhenOnline())
		return StatusTypeGroupOnline == handler->status().group();
	else
		return true;
}

void ChatWidgetMessageHandler::messageSent(const Message &message)
{
	if (!m_chatWidgetRepository)
		return;

	auto chat = message.messageChat();
	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	if (chatWidget)
		chatWidget->addMessage(message);
}

#include "moc_chat-widget-message-handler.cpp"
