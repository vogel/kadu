/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
