/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "debug.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "gui/widgets/chat-widget.h"

#include "modules/history/search/history-search-parameters.h"

#include "history-sql-storage.h"

HistorySqlStorage::HistorySqlStorage() :
		HistoryStorage()
{
	kdebugf();

	DatabaseMutex.lock();

	initDatabase();
	initQueries();

	DatabaseMutex.unlock();
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

	QSqlDatabase::removeDatabase("kadu-history");
}

void HistorySqlStorage::initDatabase()
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageDialog::msg(tr("It seems your Qt library does not provide support for selected database.\n Please select another driver in configuration window or install Qt with %1 plugin.").arg("QSQLITE"), false, "Warning");
		return;
	}

	if (QSqlDatabase::contains("kadu-history"))
	{
		if (Database.isOpen())
			Database.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	QDir historyDir(ggPath("history"));
	if (!historyDir.exists())
		historyDir.mkpath(ggPath("history"));

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(ggPath("history/history.db"));

	if (!Database.open())
	{
		MessageDialog::msg(Database.lastError().text(), false, "Warning");
		return;
	}

	initTables();
	initIndexes();
}

void HistorySqlStorage::initTables()
{
	if (!Database.tables().contains("kadu_messages"))
		initKaduMessagesTable();
	
	MessagesModel = new QSqlTableModel(0, Database);
	MessagesModel->setTable("kadu_messages");
	MessagesModel->setEditStrategy(QSqlTableModel::OnFieldChange/* OnManualSubmit*/);
}

void HistorySqlStorage::initKaduMessagesTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	executeQuery(query);

	query.prepare("PRAGMA synchronous = OFF;");
	executeQuery(query);

	query.prepare(
		"CREATE TABLE kadu_messages ("
			"chat VARCHAR(255),"
			"sender VARCHAR(255),"
			"send_time TIMESTAMP,"
			"receive_time TIMESTAMP,"
			"content TEXT,"
			"attributes TEXT);"
	);
	executeQuery(query);
}

void HistorySqlStorage::initIndexes()
{
	QSqlQuery query(Database);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat ON kadu_messages (chat)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat_receive_time ON kadu_messages (chat, receive_time)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat_receive_time_date ON kadu_messages (chat, date(receive_time))");
	executeQuery(query);
}

void HistorySqlStorage::initQueries()
{
	ClearChatHistoryQuery = QSqlQuery(Database);
	ClearChatHistoryQuery.prepare("DELETE FROM kadu_messages WHERE chat=:chat;");

	ListChatsQuery = QSqlQuery(Database);
	ListChatsQuery.prepare("SELECT DISTINCT chat FROM kadu_messages");

	ListChatDatesQuery = QSqlQuery(Database);
	ListChatDatesQuery.prepare("SELECT DISTINCT date(receive_time) as date FROM kadu_messages WHERE chat=:chat");
	
	ListChatMessagesQuery = QSqlQuery(Database);
	ListChatMessagesQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat ORDER BY receive_time");

	ListChatMessagesByDateQuery = QSqlQuery(Database);
	ListChatMessagesByDateQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat AND date(receive_time) = date(:date) ORDER BY receive_time");

	ListChatMessagesByDateLimitQuery = QSqlQuery(Database);
	ListChatMessagesByDateLimitQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat AND date(receive_time) = date(:date) ORDER BY receive_time LIMIT :limit");

	ListChatMessagesLimitQuery = QSqlQuery(Database);
	ListChatMessagesLimitQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat ORDER BY receive_time LIMIT :limit");
	
	ListChatMessagesSinceQuery = QSqlQuery(Database);
	ListChatMessagesSinceQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat AND date(receive_time) >= date(:date) ORDER BY receive_time");

	ListChatMessagesBackToQuery = QSqlQuery(Database);
	ListChatMessagesBackToQuery.prepare("SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat AND datetime(receive_time) >= datetime(:date) ORDER BY receive_time DESC LIMIT :limit");

	CountChatMessagesQuery = QSqlQuery(Database);
	CountChatMessagesQuery.prepare("SELECT COUNT(chat) FROM kadu_messages WHERE chat=:chat");

	CountChatMessagesByDateQuery = QSqlQuery(Database);
	CountChatMessagesByDateQuery.prepare("SELECT COUNT(chat) FROM kadu_messages WHERE chat=:chat AND date(receive_time) = date(:date)");
}

void HistorySqlStorage::messageReceived(const Message &message)
{
	appendMessage(message);
}

void HistorySqlStorage::messageSent(const Message &message)
{
	appendMessage(message);
}

void HistorySqlStorage::appendMessage(const Message &message)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlRecord record = MessagesModel->record();

	record.setValue("chat", message.chat().uuid().toString());
	record.setValue("sender", message.sender().uuid().toString());
	record.setValue("send_time", message.sendDate());
	record.setValue("receive_time", message.receiveDate());
	record.setValue("content", message.content());

	QString outgoing = (message.sender().ownerBuddy() == Core::instance()->myself())
			? "1"
			: "0";
	record.setValue("attributes", QString("outgoing=%1").arg(outgoing));

	MessagesModel->insertRecord(-1, record);

	DatabaseMutex.unlock();

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(Chat chat)
{
	DatabaseMutex.lock();

	ClearChatHistoryQuery.bindValue(":chat", chat.uuid().toString());
	executeQuery(ClearChatHistoryQuery);

	DatabaseMutex.unlock();
}

QList<Chat> HistorySqlStorage::chats(HistorySearchParameters search)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT chat FROM kadu_messages WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND date(receive_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(receive_time) <= date(:toDate)";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":content", QLatin1String("%") + search.query() + "%");
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<Chat> chats;

	executeQuery(query);
	while (query.next())
	{
		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat)
			chats.append(chat);
	}

	DatabaseMutex.unlock();

	return chats;
}


QList<QDate> HistorySqlStorage::chatDates(Chat chat, HistorySearchParameters search)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT date(receive_time) as date FROM kadu_messages WHERE chat=:chat";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND date(receive_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(receive_time) <= date(:toDate)";

	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	if (!search.query().isEmpty())
		query.bindValue(":content", QLatin1String("%") + search.query() + "%");
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());
	
	QList<QDate> dates;

	executeQuery(query);
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		if (date.isValid())
			dates.append(date);
	}

	DatabaseMutex.unlock();

	return dates;
}

QList<Message> HistorySqlStorage::messages(Chat chat, QDate date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QList<Message> messages;
	QSqlQuery query = date.isNull()
			? limit == 0
					? ListChatMessagesQuery
					: ListChatMessagesLimitQuery
			: limit == 0
					? ListChatMessagesByDateQuery
					: ListChatMessagesByDateLimitQuery;

	query.bindValue(":chat", chat.uuid().toString());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);
	messages = messagesFromQuery(chat, query);

	DatabaseMutex.unlock();

	return messages;
}

QList<Message> HistorySqlStorage::messagesSince(Chat chat, QDate date)
{
	kdebugf();
	
	DatabaseMutex.lock();
	
	QList<Message> messages;
	if (date.isNull())
		return messages;
	
	ListChatMessagesSinceQuery.bindValue(":chat", chat.uuid().toString());
	ListChatMessagesSinceQuery.bindValue(":date", date.toString(Qt::ISODate));
	executeQuery(ListChatMessagesSinceQuery);
	messages = messagesFromQuery(chat, ListChatMessagesSinceQuery);
	
	DatabaseMutex.unlock();
	
	return messages;
}

QList<Message> HistorySqlStorage::messagesBackTo(Chat chat, QDateTime datetime, int limit)
{
	DatabaseMutex.lock();

	QList<Message> result;
	QSqlQuery query = ListChatMessagesBackToQuery;

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", datetime.toString(Qt::ISODate));
	query.bindValue(":limit", limit);
	executeQuery(query);
	result = messagesFromQuery(chat, query);

	DatabaseMutex.unlock();

	QList<Message> messages;
	for (int i = result.size() - 1; i >= 0; --i)
		messages.append(result.at(i));

	return messages;
}

int HistorySqlStorage::messagesCount(Chat chat, QDate date)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query = date.isNull()
			? CountChatMessagesQuery
			: CountChatMessagesByDateQuery;

	query.bindValue(":chat", chat.uuid().toString());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
	query.next();

	DatabaseMutex.unlock();

	return query.value(0).toInt();
}

void HistorySqlStorage::executeQuery(QSqlQuery query)
{
	kdebugf();

	query.exec();
	kdebug("db query: %s\n", qPrintable(query.executedQuery()));
}


QList<Message> HistorySqlStorage::messagesFromQuery(Chat chat, QSqlQuery query)
{
	QList<Message> messages;

	while (query.next())
	{
		bool outgoing = QVariant(query.value(4).toString().split('=').last()).toBool();
		Message::Type type = outgoing ? Message::TypeSent : Message::TypeReceived;
		Buddy senderBuddy = outgoing ? Core::instance()->myself() : BuddyManager::instance()->byUuid(query.value(0).toString());
		QList<Contact> contacts = senderBuddy.contacts(chat.chatAccount());
		Contact sender;

		if (!contacts.isEmpty())
			sender = contacts[0];
		else
			continue;
		
		Message message(chat, type, sender);
		message
			.setContent(query.value(1).toString())
			.setSendDate(query.value(2).toDateTime())
			.setReceiveDate(query.value(3).toDateTime())
			.setStatus(outgoing ? Message::StatusDelivered : Message::StatusReceived);
		
		messages.append(message);
	}

	return messages;
}
