/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include "icons/icons-manager.h"
#include "message/message.h"
#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "new-message-notification.h"

NotifyEvent *MessageNotification::NewChatNotifyEvent = 0;
NotifyEvent *MessageNotification::NewMessageNotifyEvent = 0;

void MessageNotification::registerEvents()
{
	if (NewChatNotifyEvent)
		return;

	NewChatNotifyEvent = new NotifyEvent("NewChat", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "New chat"));
	NewMessageNotifyEvent = new NotifyEvent("NewMessage", NotifyEvent::CallbackNotRequired,
			QT_TRANSLATE_NOOP("@default", "New message"));

	NotificationManager::instance()->registerNotifyEvent(NewChatNotifyEvent);
	NotificationManager::instance()->registerNotifyEvent(NewMessageNotifyEvent);
}

void MessageNotification::unregisterEvents()
{
	if (!NewChatNotifyEvent)
		return;

	NotificationManager::instance()->unregisterNotifyEvent(NewChatNotifyEvent);
	NotificationManager::instance()->unregisterNotifyEvent(NewMessageNotifyEvent);

	delete NewChatNotifyEvent;
	NewChatNotifyEvent = 0;

	delete NewMessageNotifyEvent;
	NewMessageNotifyEvent = 0;
}

MessageNotification::MessageNotification(MessageType messageType, const Message &message) :
		ChatNotification(message.messageChat(), messageType == NewChat ? "NewChat" : "NewMessage",
		KaduIcon("protocols/common/message"))
{
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
	setDetails(message.content());
}

