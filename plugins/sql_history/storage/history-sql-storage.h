/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef HISTORY_SQL_STORAGE_H
#define HISTORY_SQL_STORAGE_H

#include <QtCore/QMutex>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "plugins/history/storage/history-storage.h"

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QSqlDatabase Database;

	QSqlQuery ListChatsQuery;
	QSqlQuery AppendMessageQuery;
	QSqlQuery AppendStatusQuery;
	QSqlQuery AppendSmsQuery;

	QMutex DatabaseMutex;

	QMap<Chat, int> ChatMap;
	QMap<Contact, int> ContactMap;
	QMap<QString, int> DateMap;

	void copyHistoryFile();

	void initDatabase();
	void initQueries();
	void initTables();
	void initIndexes();
	void initKaduSchemaTable();
	void initKaduMessagesTable();
	void initKaduStatusesTable();
	void initKaduSmsTable();

	quint16 loadSchemaVersion();

	void importVersion1Schema();

	int findOrCreateChat(const Chat &chat);
	int findOrCreateContact(const Contact &contact);
	int saveMessageContent(const Message &message);
	int findOrCreateDate(const QDate &date);

	QString chatWhere(const Chat &chat);
	QString buddyContactsWhere(const Buddy &buddy);

	void executeQuery(QSqlQuery &query);
	QVector<Message> messagesFromQuery(QSqlQuery &query);
	QList<TimedStatus> statusesFromQuery(QSqlQuery &query);
	QVector<Message> smsFromQuery(QSqlQuery &query);

private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

public:
	explicit HistorySqlStorage(QObject *parent = 0);
	virtual ~HistorySqlStorage();

	virtual QVector<Chat> chats(const HistorySearchParameters &search);

	virtual QVector<DatesModelItem> chatDates(const Chat &chat, const HistorySearchParameters &search);
	virtual QVector<Message> messages(const Chat &chat, const QDate &date = QDate(), int limit = 0);
	virtual QVector<Message> messagesSince(const Chat &chat, const QDate &date);
	virtual QVector<Message> messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit);

	virtual QVector<Buddy> statusBuddiesList(const HistorySearchParameters &search);
	virtual QVector<DatesModelItem> datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search);
	virtual QList<TimedStatus> statuses(const Buddy &buddy, const QDate &date = QDate(), int limit = 0);

	virtual QList<QString> smsRecipientsList(const HistorySearchParameters &search);
	virtual QVector<DatesModelItem> datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search);
	virtual QVector<Message> sms(const QString &recipient, const QDate &date = QDate(), int limit = 0);

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
