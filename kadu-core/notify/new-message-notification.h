/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef NEW_MESSAGE_NOTIFICATION_H
#define NEW_MESSAGE_NOTIFICATION_H

#include "chat-notification.h"

class Message;
class NotifyEvent;

class MessageNotification : public ChatNotification
{
	Q_OBJECT

	static NotifyEvent *NewChatNotifyEvent;
	static NotifyEvent *NewMessageNotifyEvent;

public:
	enum MessageType {
		NewChat,
		NewMessage
	};

	static void registerEvents();
	static void unregisterEvents();

	MessageNotification(MessageType messageType, const Message &message);
	virtual ~MessageNotification() {}

};

#endif // NEW_MESSAGE_NOTIFICATION_H
