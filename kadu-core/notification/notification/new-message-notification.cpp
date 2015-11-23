/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTextDocument>

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "icons/icons-manager.h"
#include "message/message.h"
#include "notification/notification-manager.h"
#include "notification/notification-event.h"
#include "notification/notification-event-repository.h"

#include "new-message-notification.h"

void MessageNotification::registerEvents()
{
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("NewChat", QT_TRANSLATE_NOOP("@default", "New chat")));
	Core::instance()->notificationEventRepository()->addNotificationEvent(NotificationEvent("NewMessage", QT_TRANSLATE_NOOP("@default", "New message")));
}

void MessageNotification::unregisterEvents()
{
	if (Core::instance()) // TODO: hack
	{
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("NewChat", QT_TRANSLATE_NOOP("@default", "New chat")));
		Core::instance()->notificationEventRepository()->removeNotificationEvent(NotificationEvent("NewMessage", QT_TRANSLATE_NOOP("@default", "New message")));
	}
}

MessageNotification::MessageNotification(ChatWidgetRepository *chatWidgetRepository, MessageType messageType, const Message &message) :
		Notification(Account::null, message.messageChat(), messageType == NewChat ? "NewChat" : "NewMessage",
		KaduIcon("protocols/common/message")), CurrentMessage(message)
{
	addChatCallbacks();

	connect(chatWidgetRepository, SIGNAL(chatWidgetAdded(ChatWidget*)), this, SLOT(chatWidgetAdded(ChatWidget*)));

	QString syntax;

	if (messageType == NewChat)
	{
		setTitle(tr("New chat"));
		syntax = tr("Chat with <b>%1</b>");
	}
	else
	{
		setTitle(tr("New message"));
		syntax = tr("New message from <b>%1</b>");
	}

	setText(syntax.arg(Qt::escape(message.messageSender().display(true))));
	setDetails(message.htmlContent());
}

void MessageNotification::chatWidgetAdded(ChatWidget *chatWidget)
{
	if (chatWidget->chat() == data()["chat"].value<Chat>())
		close();
}

#include "moc_new-message-notification.cpp"
