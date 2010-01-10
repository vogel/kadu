/*
 * %kadu copyright begin%
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_NOTIFICATION_H
#define CHAT_NOTIFICATION_H

#include "account-notification.h"

#include "chat/chat.h"

class ChatNotification : public AccountNotification
{
	Chat CurrentChat;

public:
	ChatNotification(Chat chat, const QString &type, const QIcon &icon);
	virtual ~ChatNotification() {}

	Chat chat() { return CurrentChat; }
};

#endif // CHAT_NOTIFICATION_H
