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

#include "message-notification-service.h"

#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "message/message.h"
#include "misc/memory.h"
#include "notification/notification/notification.h"
#include "notification/notification-callback-repository.h"
#include "notification/notification-callback.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "notification/notification-service.h"

MessageNotificationService::MessageNotificationService(QObject *parent) :
		QObject{parent},
		m_openChatCallback{QStringLiteral("chat-open"), tr("Chat"),
			[this](Notification *notification){ return openChat(notification); }},
		m_newChatEvent{QStringLiteral("NewChat"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "New chat"))},
		m_newMessageEvent{QStringLiteral("NewMessage"), QStringLiteral(QT_TRANSLATE_NOOP("@default", "New message"))}
{
}

MessageNotificationService::~MessageNotificationService()
{
}

void MessageNotificationService::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void MessageNotificationService::setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository)
{
	m_notificationCallbackRepository = notificationCallbackRepository;
}

void MessageNotificationService::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void MessageNotificationService::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void MessageNotificationService::init()
{
	m_notificationEventRepository->addNotificationEvent(m_newChatEvent);
	m_notificationEventRepository->addNotificationEvent(m_newMessageEvent);

	m_notificationCallbackRepository->addCallback(m_openChatCallback);
}

void MessageNotificationService::done()
{
	m_notificationEventRepository->removeNotificationEvent(m_newChatEvent);
	m_notificationEventRepository->removeNotificationEvent(m_newMessageEvent);

	m_notificationCallbackRepository->removeCallback(m_openChatCallback);
}

void MessageNotificationService::notifyNewChat(const Message &message)
{
	auto data = QVariantMap{};
	data.insert(QStringLiteral("account"), qVariantFromValue(message.messageChat().chatAccount()));
	data.insert(QStringLiteral("chat"), qVariantFromValue(message.messageChat()));

	auto notification = make_unique<Notification>(data, m_newChatEvent.name(), KaduIcon{});
	notification->setTitle(tr("New chat"));
	notification->setText(tr("Chat with <b>%1</b>").arg(Qt::escape(message.messageSender().display(true))));
	notification->setDetails(message.htmlContent());
	notification->addCallback(m_openChatCallback.name());
	notification->addCallback("ignore");

	m_notificationService->notify(notification.release());
}

void MessageNotificationService::notifyNewMessage(const Message &message)
{
	auto data = QVariantMap{};
	data.insert(QStringLiteral("account"), qVariantFromValue(message.messageChat().chatAccount()));
	data.insert(QStringLiteral("chat"), qVariantFromValue(message.messageChat()));

	auto notification = make_unique<Notification>(data, m_newMessageEvent.name(), KaduIcon{});
	notification->setTitle(tr("New message"));
	notification->setText(tr("New message from <b>%1</b>").arg(Qt::escape(message.messageSender().display(true))));
	notification->setDetails(message.htmlContent());
	notification->addCallback(m_openChatCallback.name());
	notification->addCallback("ignore");

	m_notificationService->notify(notification.release());
}

void MessageNotificationService::openChat(Notification *notification)
{
	auto chat = qvariant_cast<Chat>(notification->data()["chat"]);
	if (chat)
		m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
}

#include "moc_message-notification-service.cpp"
