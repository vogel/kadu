/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QObject>

#include "chat/chat.h"
#include "buddies/buddy.h"

#include "../history_exports.h"

class HistorySearchParameters;
class Message;

class HISTORYAPI HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(const Message &message) = 0;
	virtual void messageSent(const Message &message) = 0;

public:
	explicit HistoryStorage(QObject *parent) : QObject(parent) {}
	virtual ~HistoryStorage() {}

	virtual QList<Chat> chats(HistorySearchParameters search) = 0;

	virtual QList<QDate> chatDates(Chat chat, HistorySearchParameters search) = 0;
	virtual QList<Message> messages(Chat chat, QDate date = QDate(), int limit = 0) = 0;
	virtual QList<Message> messagesSince(Chat chat, QDate date) = 0;
	virtual QList<Message> messagesBackTo(Chat chat, QDateTime datetime, int limit) = 0;
	virtual int messagesCount(Chat chat, QDate date = QDate()) = 0;

	virtual void appendMessage(const Message &message) = 0;
	virtual void sync() = 0;

	virtual void clearChatHistory(Chat chat) = 0;

};

#endif // HISTORY_STORAGE_H
