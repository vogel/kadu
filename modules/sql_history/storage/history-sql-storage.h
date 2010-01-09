/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

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
	QTimer *CommitTimer;

	QSqlQuery ListChatsQuery;
	QSqlQuery AppendMessageQuery;

	QMutex DatabaseMutex;

	void initDatabase();
	void initQueries();
	void initTables();
	void initIndexes();
	void initKaduMessagesTable();

	QString chatWhere(Chat chat);

	void executeQuery(QSqlQuery query);
	QList<Message> messagesFromQuery(Chat chat, QSqlQuery query);

private slots:
	void newTransaction();

	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

protected:
	virtual void crash();

public:
	explicit HistorySqlStorage(QObject *parent = 0);
	virtual ~HistorySqlStorage();

	virtual QList<Chat> chats(HistorySearchParameters search);
	virtual QList<QDate> chatDates(Chat chat, HistorySearchParameters search);
	virtual QList<Message> messages(Chat chat, QDate date = QDate(), int limit = 0);
	virtual QList<Message> messagesSince(Chat chat, QDate date);
	virtual QList<Message> messagesBackTo(Chat chat, QDateTime datetime, int limit);

	virtual int messagesCount(Chat chat, QDate date = QDate());

	void appendMessage(const Message &message);
	void clearChatHistory(Chat chat);
	
	//TODO 0.6.6
	void convertSenderToContact();

};

#endif // HISTORY_SQL_STORAGE_H
