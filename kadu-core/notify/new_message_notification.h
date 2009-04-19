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

#include "account-notification.h"

class ContactSet;
class NotifyEvent;

class MessageNotification : public AccountNotification
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

	MessageNotification(MessageType messageType, const ContactSet &contacts, const QString &message, Account *account);
	virtual ~MessageNotification() {};

};

#endif // NEW_MESSAGE_NOTIFICATION_H
