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

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>

#include "modules/history/storage/history-storage.h"

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QSqlDatabase Database;
	QSqlTableModel *MessagesModel;

	QSqlQuery ClearChatHistoryQuery;
	QSqlQuery ListChatsQuery;
	QSqlQuery ListChatDatesQuery;
	QSqlQuery ListChatMessagesQuery;
	QSqlQuery ListChatMessagesByDateQuery;
	QSqlQuery ListChatMessagesByDateLimitQuery;
	QSqlQuery ListChatMessagesLimitQuery;
	QSqlQuery CountChatMessagesQuery;
	QSqlQuery CountChatMessagesByDateQuery;
	
	void initDatabase();
	void initQueries();
	void initTables();
	void initKaduMessagesTable();

	void executeQuery(QSqlQuery query);

private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

public:
	HistorySqlStorage();
	~HistorySqlStorage();

	virtual QList<Chat *> chats();
	virtual QList<QDate> chatDates(Chat *chat);
	virtual QList<Message> messages(Chat *chat, QDate date = QDate(), int limit = 0);

	virtual int messagesCount(Chat *chat, QDate date = QDate());

	void appendMessage(const Message &message);
	void clearChatHistory(Chat *chat);

};

#endif // HISTORY_SQL_STORAGE_H
