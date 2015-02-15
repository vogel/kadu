/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef CHAT_NOTIFICATION_H
#define CHAT_NOTIFICATION_H

#include "account-notification.h"
#include "exports.h"

#include "chat/chat.h"

class KADUAPI ChatNotification : public AccountNotification
{
	Q_OBJECT

	Chat CurrentChat;

public:
	ChatNotification(const Chat &chat, const QString &type, const KaduIcon &icon, bool showChatCallbacks);
	virtual ~ChatNotification() {}

	Chat chat() { return CurrentChat; }

	virtual QString groupKey() const { return CurrentChat.uuid().toString(); }

public slots:
	void openChat();

	virtual void callbackAccept();

};

#endif // CHAT_NOTIFICATION_H
