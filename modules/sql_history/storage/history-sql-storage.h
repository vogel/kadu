/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_SQL_STORAGE_H
#define HISTORY_SQL_STORAGE_H

#include <QtCore/QMutex>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "core/crash-aware-object.h"

#include "modules/history/storage/history-storage.h"

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage, CrashAwareObject
{
	Q_OBJECT

	QSqlDatabase Database;

	QSqlQuery ListChatsQuery;
	QSqlQuery AppendMessageQuery;
	QSqlQuery AppendStatusQuery;
	QSqlQuery AppendSmsQuery;

	QMutex DatabaseMutex;

	void initDatabase();
	void initQueries();
	void initTables();
	void initIndexes();
	void initKaduMessagesTable();
	void initKaduStatusesTable();
	void initKaduSmsTable();

	QString chatWhere(const Chat &chat);
	QString buddyContactsWhere(const Buddy &buddy);

	void executeQuery(QSqlQuery &query);
	QList<Message> messagesFromQuery(QSqlQuery &query);
	QList<TimedStatus> statusesFromQuery(QSqlQuery query);
	QList<Message> smsFromQuery(QSqlQuery query);

private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

protected:
	virtual void crash();

public:
	explicit HistorySqlStorage(QObject *parent = 0);
	virtual ~HistorySqlStorage();

	virtual QList<Chat> chats(const HistorySearchParameters &search);

	virtual QList<QDate> chatDates(const Chat &chat, const HistorySearchParameters &search);
	virtual QList<Message> messages(const Chat &chat, const QDate &date = QDate(), int limit = 0);
	virtual QList<Message> messagesSince(const Chat &chat, const QDate &date);
	virtual QList<Message> messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit);
	virtual QPair<int, Message> firstMessageAndCount(const Chat &chat, const QDate &date);

	virtual QList<Buddy> statusBuddiesList(const HistorySearchParameters &search);
	virtual QList<QDate> datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search);
	virtual QList<TimedStatus> statuses(const Buddy &buddy, const QDate &date = QDate(), int limit = 0);
	virtual int statusBuddyCount(const Buddy &buddy, const QDate &date = QDate());

	virtual int messagesCount(const Chat &chat, const QDate &date = QDate());

	virtual QList<QString> smsRecipientsList(const HistorySearchParameters &search);
	virtual QList<QDate> datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search);
	virtual QList<Message> sms(const QString &recipient, const QDate &date = QDate(), int limit = 0);
	virtual int smsCount(const QString &recipient, const QDate &date = QDate());

	virtual void appendMessage(const Message &message);
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time = QDateTime::currentDateTime());
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time = QDateTime::currentDateTime());

	void sync();

	virtual void clearChatHistory(const Chat &chat, const QDate &date = QDate());
	virtual void clearSmsHistory(const QString &recipient, const QDate &date = QDate());
	virtual void clearStatusHistory(const Buddy &buddy, const QDate &date = QDate());
	virtual void deleteHistory(const Buddy &buddy);

};

#endif // HISTORY_SQL_STORAGE_H
