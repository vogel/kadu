/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "message/message-manager.h"
#include "message/message.h"
#include "notify/notification/new-message-notification.h"
#include "services/notification-service.h"
#include "activate.h"

#include "chat-event-listener.h"

ChatEventListener::ChatEventListener(NotificationService *service)
		: EventListener(service)
{
	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
}

ChatEventListener::~ChatEventListener()
{
	disconnect(MessageManager::instance(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
}

void ChatEventListener::messageReceived(const Message &message)
{
	auto chatWidget = Core::instance()->chatWidgetRepository()->widgetForChat(message.messageChat());

	if (!chatWidget)
		Service->notify(new MessageNotification(Core::instance()->chatWidgetRepository(), MessageNotification::NewChat, message));
	else if (!Service->newMessageOnlyIfInactive() || !_isWindowActiveOrFullyVisible(chatWidget))
		Service->notify(new MessageNotification(Core::instance()->chatWidgetRepository(), MessageNotification::NewMessage, message));
}

#include "moc_chat-event-listener.cpp"
