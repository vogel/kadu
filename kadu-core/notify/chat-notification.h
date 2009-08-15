/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_NOTIFICATION_H
#define CHAT_NOTIFICATION_H

#include "account-notification.h"

class Chat;

class ChatNotification : public AccountNotification
{

	Chat *CurrentChat;
public:
	ChatNotification(Chat *chat, const QString &type, const QIcon &icon);
	virtual ~ChatNotification() {}

	Chat *chat() { return CurrentChat; }
};

#endif // CHAT_NOTIFICATION_H
