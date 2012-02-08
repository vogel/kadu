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

class QSqlError;

class HistoryQuery;
class ProgressWindow2;

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QThread *InitializerThread;
	ProgressWindow2 *ImportProgressWindow;

	QSqlDatabase Database;

	QSqlQuery AppendMessageQuery;
	QSqlQuery AppendStatusQuery;
	QSqlQuery AppendSmsQuery;

	QMutex DatabaseMutex;

	QMap<Chat, int> ChatMap;
	QMap<Contact, int> ContactMap;
	QMap<QString, int> DateMap;

	HistoryMessagesStorage *ChatStorage;
	HistoryMessagesStorage *StatusStorage;
	HistoryMessagesStorage *SmsStorage;

	void initQueries();

	int findOrCreateChat(const Chat &chat);
	Chat findChat(int id);
	int findOrCreateContact(const Contact &contact);
	int saveMessageContent(const Message &message);
	int findOrCreateDate(const QDate &date);

	QString chatWhere(const Chat &chat, const QString &chatPrefix = "chat.");
	QString talkableContactsWhere(const Talkable &talkable, const QString &fieldName);
	QString buddyContactsWhere(const Buddy &buddy, const QString &fieldName);

	void executeQuery(QSqlQuery &query);
	QVector<Message> messagesFromQuery(QSqlQuery &query);
	QVector<Message> statusesFromQuery(const Contact &contact, QSqlQuery &query);
	QVector<Message> smsFromQuery(QSqlQuery &query);

	bool isDatabaseReady();
	bool waitForDatabase();

	QVector<Talkable> syncChats();
	QVector<Talkable> syncStatusBuddies();
	QVector<Talkable> syncSmsRecipients();

	QVector<HistoryQueryResult> syncChatDates(const HistoryQuery &historyQuery);
	QVector<HistoryQueryResult> syncStatusDates(const HistoryQuery &historyQuery);
	QVector<HistoryQueryResult> syncSmsRecipientDates(const HistoryQuery &historyQuery);

	QVector<Message> syncMessages(const HistoryQuery &historyQuery);
	QVector<Message> syncStatuses(const HistoryQuery &historyQuery);
	QVector<Message> syncSmses(const HistoryQuery &historyQuery);

private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

	void databaseReady(bool ok);

	void importStarted();
	void importFinished();
	void databaseOpenFailed(const QSqlError &error);

public:
	explicit HistorySqlStorage(QObject *parent = 0);
	virtual ~HistorySqlStorage();

	virtual QFuture<QVector<Talkable> > chats();
	virtual QFuture<QVector<Talkable> > statusBuddies();
	virtual QFuture<QVector<Talkable> > smsRecipients();

	virtual QFuture<QVector<HistoryQueryResult> > chatDates(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<HistoryQueryResult> > statusDates(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<HistoryQueryResult> > smsRecipientDates(const HistoryQuery &historyQuery);

	virtual QFuture<QVector<Message> > messages(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<Message> > statuses(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<Message> > smses(const HistoryQuery &historyQuery);

	virtual void appendMessage(const Message &message);
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time);
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time);

	void sync();

	virtual void clearChatHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearSmsHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearStatusHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void deleteHistory(const Talkable &talkable);

	virtual HistoryMessagesStorage * chatStorage();
	virtual HistoryMessagesStorage * statusStorage();
	virtual HistoryMessagesStorage * smsStorage();

};

#endif // HISTORY_SQL_STORAGE_H
