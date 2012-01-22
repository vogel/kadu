/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtCore/QDateTime>
#include <QtCore/QFuture>
#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "chat/chat.h"

#include "../history_exports.h"

struct DatesModelItem;
class Message;
class Status;
class TimedStatus;

class HISTORYAPI HistoryStorage : public QObject
{
	Q_OBJECT

private slots:
	virtual void messageReceived(const Message &message) = 0;
	virtual void messageSent(const Message &message) = 0;

public:
	explicit HistoryStorage(QObject *parent) : QObject(parent) {}
	virtual ~HistoryStorage() {}

	virtual QFuture<QVector<Chat> > chats() = 0;
	virtual QFuture<QVector<Buddy> > statusBuddies() = 0;
	virtual QFuture<QVector<QString> > smsRecipients() = 0;

	virtual QFuture<QVector<DatesModelItem> > chatDates(const Chat &chat) = 0;
	virtual QFuture<QVector<DatesModelItem> > statusBuddyDates(const Buddy &buddy) = 0;
	virtual QFuture<QVector<DatesModelItem> > statusContactDates(const Contact &contact) = 0;
	virtual QFuture<QVector<DatesModelItem> > smsRecipientDates(const QString &recipient) = 0;

	virtual QFuture<QVector<Message> > messages(const Chat &chat, const QDate &date = QDate(), int limit = 0) = 0;
	virtual QFuture<QVector<Message> > messagesSince(const Chat &chat, const QDate &date) = 0;
	virtual QFuture<QVector<Message> > messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit) = 0;

	virtual QFuture<QVector<Message> > statuses(const Buddy &buddy, const QDate &date = QDate(), int limit = 0) = 0;
	virtual QFuture<QVector<Message> > statuses(const Contact &contact, const QDate &date = QDate(), int limit = 0) = 0;

	virtual QFuture<QVector<Message> > smses(const QString &recipient, const QDate &date = QDate(), int limit = 0) = 0;

	virtual void appendMessage(const Message &message) = 0;
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time = QDateTime::currentDateTime()) = 0;
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time = QDateTime::currentDateTime()) = 0;
	virtual void sync() = 0;

	virtual void clearChatHistory(const Chat &chat, const QDate &date = QDate()) = 0;
	virtual void clearSmsHistory(const QString &recipient, const QDate &date = QDate()) = 0;
	virtual void clearStatusHistory(const Buddy &buddy, const QDate &date = QDate()) = 0;
	virtual void deleteHistory(const Buddy &buddy) = 0;

};

#endif // HISTORY_STORAGE_H
