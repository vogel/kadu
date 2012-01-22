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

	void initQueries();

	int findOrCreateChat(const Chat &chat);
	int findOrCreateContact(const Contact &contact);
	int saveMessageContent(const Message &message);
	int findOrCreateDate(const QDate &date);

	QString chatWhere(const Chat &chat, const QString &chatPrefix = "chat.");
	QString talkableContactsWhere(const Talkable &talkable, const QString &fieldName);
	QString buddyContactsWhere(const Buddy &buddy, const QString &fieldName);

	void executeQuery(QSqlQuery &query);
	QVector<Message> messagesFromQuery(QSqlQuery &query);
	QVector<Message> statusesFromQuery(QSqlQuery &query);
	QVector<Message> smsFromQuery(QSqlQuery &query);

	bool isDatabaseReady();
	bool waitForDatabase();

	QVector<Chat> syncChats();
	QVector<Buddy> syncStatusBuddies();
	QVector<QString> syncSmsRecipients();

	QVector<DatesModelItem> syncChatDates(const Talkable &talkable);
	QVector<DatesModelItem> syncStatusDates(const Talkable &talkable);
	QVector<DatesModelItem> syncSmsRecipientDates(const Talkable &talkable);

	QVector<Message> syncMessages(const Talkable &talkable, const QDate &date);
	QVector<Message> syncMessagesSince(const Chat &chat, const QDate &date);
	QVector<Message> syncMessagesBackTo(const Chat &chat, const QDateTime &datetime, int limit);

	QVector<Message> syncStatuses(const Talkable &talkable, const QDate &date);
	QVector<Message> syncSmses(const Talkable &talkable, const QDate &date);

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

	virtual QFuture<QVector<Chat> > chats();
	virtual QFuture<QVector<Buddy> > statusBuddies();
	virtual QFuture<QVector<QString> > smsRecipients();

	virtual QFuture<QVector<DatesModelItem> > chatDates(const Talkable &talkable);
	virtual QFuture<QVector<DatesModelItem> > statusDates(const Talkable &talkable);
	virtual QFuture<QVector<DatesModelItem> > smsRecipientDates(const Talkable &talkable);

	virtual QFuture<QVector<Message> > messages(const Talkable &talkable, const QDate &date);
	virtual QFuture<QVector<Message> > messagesSince(const Chat &chat, const QDate &date);
	virtual QFuture<QVector<Message> > messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit);

	virtual QFuture<QVector<Message> > statuses(const Talkable &talkable, const QDate &date);

	virtual QFuture<QVector<Message> > smses(const Talkable &talkable, const QDate &date);

	virtual void appendMessage(const Message &message);
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time);
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time);

	void sync();

	virtual void clearChatHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearSmsHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearStatusHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void deleteHistory(const Talkable &talkable);

};

#endif // HISTORY_SQL_STORAGE_H
