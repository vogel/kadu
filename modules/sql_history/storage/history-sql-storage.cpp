/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "contacts/contact-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "debug.h"
#include "gui/windows/message-box.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "gui/widgets/chat-widget.h"

#include "history-sql-storage.h"

HistorySqlStorage::HistorySqlStorage() :
		HistoryStorage(), MessagesAdded(0)
{
	initDatabase();
	initQueries();
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

	submitRemaining();
	QSqlDatabase::removeDatabase("kadu-history");

	kdebugf2();
}

void HistorySqlStorage::initDatabase()
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageBox::msg(tr("It seems your Qt library does not provide support for selected database.\n Please select another driver in configuration window or install Qt with %1 plugin.").arg("QSQLITE"), false, "Warning");
		return;
	}

	if (QSqlDatabase::contains("kadu-history"))
	{
		if (Database.isOpen())
			Database.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(ggPath("history/history.db"));

	if (!Database.open())
	{
		MessageBox::msg(Database.lastError().text(), false, "Warning");
		return;
	}

	initTables();
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

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat ON kadu_messages (chat)");
	executeQuery(query);
	
	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat_receive_time ON kadu_messages (chat, receive_time)");
	executeQuery(query);
}

void HistorySqlStorage::initQueries()
{
	ClearChatHistoryQuery = QSqlQuery(Database);
	ClearChatHistoryQuery.prepare("DELETE FROM kadu_messages WHERE chat=:chat;");
}

void HistorySqlStorage::submitRemaining()
{
	submitRemainingKaduMessages();
}

void HistorySqlStorage::submitRemainingKaduMessages()
{
	if (MessagesModel)
		MessagesModel->submitAll();
}

void HistorySqlStorage::messageReceived(const Message &message)
{
	appendMessage(message);
}

void HistorySqlStorage::messageSent(const Message &message)
{
	appendMessage(message);
}


// void HistorySqlStorage::statusChanged(Contact elem, QString protocolName, const UserStatus & oldStatus, bool /*massively*/, bool /*last*/)
// {
// 	kdebugf();
// 	//�ohoho...
// 	if (!config_file.readBoolEntry("History", "Enable") || !config_file.readBoolEntry("History", "SaveStatusChanges") || (!elem.data("history_save_status").toBool() && !config_file.readBoolEntry("History", "SaveStatusChangesForAll")) || (config_file.readBoolEntry("History", "IgnoreSomeStatusChanges") && ((elem.status(protocolName).isOnline() && oldStatus.isBusy()) || (elem.status(protocolName).isBusy() && oldStatus.isOnline()))) || (!elem.status(protocolName).hasDescription() && config_file.readBoolEntry("History", "SaveOnlyStatusWithDescription")) || elem.status(protocolName) == oldStatus && elem.status(protocolName).description() != oldStatus.description()) 
// 	{
// 		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
// 		return;
// 	}
// 	appendStatus(elem, protocolName);
// 	kdebugf2();
// }

void HistorySqlStorage::appendStatus(Contact elem, QString protocolName)
{
	kdebugf();
/*	QHostAddress ip;
	unsigned short port;
	QString desc, query_str;
	if(Database.driverName() == "QSQLITE")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', datetime('now', 'localtime'), '%3', '%4');";
	else if(Database.driverName() == "QMYSQL" || Database.driverName() == "QPSQL")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', NOW(), '%3', '%4');";
	QString uid_group_str = findUidGroup(ContactList(elem));
	if (uid_group_str == "")
		uid_group_str = addUidGroup(ContactList(elem));
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
// 	if (userlist->contains(protocolName, elem.ID(protocolName), FalseForAnonymous))
// 	{
// 		Contact user = userlist->byID(protocolName, elem.ID(protocolName));
// 		ip = user.IP(protocolName);
// 		port = user.port(protocolName);
// 	}
// 	else
// 	{
		ip.setAddress((unsigned int)0);
		port = 0;
// 	}
	QString addr = ip.toString();
	if (port)
		addr = addr + QString(":") + QString::number(port);
// 	if (elem.status(protocolName).hasDescription())
// 	{
// 		desc = elem.status(protocolName).description();
// 		HtmlDocument::escapeText(desc);
// 	}

	query_str = query_str.arg(uid_group_str).arg(QString()).arg(QString(desc.toUtf8().data())).arg(QString(addr.toUtf8().data()));
	executeQuery(query_str);
*/
	kdebugf2();
}
//TODO: cache and optimize
void HistorySqlStorage::appendMessage(const Message &message)
{
	kdebugf();

	QSqlRecord record = MessagesModel->record();

	record.setValue("chat", message.chat()->uuid().toString());
	record.setValue("sender", message.sender().uuid().toString());
	record.setValue("send_time", message.sendDate());
	record.setValue("receive_time", message.receiveDate());
	record.setValue("content", message.content());

	QString outgoing = (message.sender() == Core::instance()->myself())
			? "1"
			: "0";
	record.setValue("attributes", QString("outgoing=%1").arg(outgoing));

	MessagesModel->insertRecord(-1, record);

	MessagesAdded++;

	if (1000 <= MessagesAdded)
		if (!MessagesModel->submitAll())
			kdebug(Database.lastError().text().toLocal8Bit().data(), false, "Warning");

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(Chat *chat)
{
	ClearChatHistoryQuery.bindValue(":chat", chat->uuid().toString());
	executeQuery(ClearChatHistoryQuery);
}

void HistorySqlStorage::appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time)
{
	kdebugf();
/*	QString query_str;
	if(Database.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', datetime('%3', 'unixepoch', 'localtime'), datetime('%4', 'unixepoch', 'localtime'), '%5');";
	else if(Database.driverName() == "QMYSQL" || Database.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '%2', FROM_UNIXTIME('%3'), FROM_UNIXTIME('%4'), '%5');";

	QString uid_group_str = findUidGroup(list);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(list);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString message_str = prepareText(msg);
	QString outstr = QString::number(outgoing);
	QString send_time_str = QString::number(send_time);
	QString rec_time_str = QString::number(receive_time);
	query_str = query_str.arg(uid_group_str).arg(outstr).arg(send_time_str).arg(rec_time_str).arg(QString(message_str.toUtf8()));
	executeQuery(query_str);
*/
	kdebugf2();
}


void HistorySqlStorage::appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &ip_port)
{
	kdebugf();
/*	QString query_str;
	if(Database.driverName() == "QSQLITE")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', datetime('%3', 'unixepoch', 'localtime'), '%4', '%5');";
	else if(Database.driverName() == "QMYSQL" || Database.driverName() == "QPSQL")
		query_str  = "INSERT INTO kadu_status (uid_group_id, status, time, description, ip_port) VALUES ('%1', '%2', FROM_UNIXTIME('%3'), '%4', '%5');";
	QString uid_group_str = findUidGroup(list);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(list);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	query_str = query_str.arg(uid_group_str).arg(status).arg(QString::number(time)).arg(QString(desc.toUtf8().data())).arg(ip_port.toUtf8().data());
	executeQuery(query_str);
*/
	kdebugf2();


}

void HistorySqlStorage::appendSms(const QString &mobile, const QString &msg)
{
	kdebugf();
/*
	QString query_str;
	if(Database.driverName() == "QSQLITE")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', datetime('now', 'localtime'), datetime('now', 'localtime'), '%2');";
	else if(Database.driverName() == "QMYSQL" || Database.driverName() == "QPSQL")
		query_str = "INSERT INTO kadu_sms (uid_group_id, is_outgoing, send_time, receive_time, content) VALUES ('%1', '1', NOW(), NOW(), '%2');";
	Contact mob = Contact();
//	mob.addProtocol("SMS", mobile);
	ContactList mobs = ContactList(mob);
	QString uid_group_str = findUidGroup(mobs);
	if (uid_group_str == "")
		uid_group_str = addUidGroup(mobs);
	if (uid_group_str == "")
	{
		kdebugf2();
		return;
	}
	QString message_str = prepareText(msg);
	query_str = query_str.arg(uid_group_str).arg(QString(message_str.toUtf8()));

	executeQuery(query_str);
*/
	kdebugf2();
}

void HistorySqlStorage::removeHistory(const ContactList& uids, const QDate &date, HistoryEntryType type)
{
	kdebugf();
/*
	QString uid_group_str = findUidGroup(uids);
	if(date.isValid())
	{
		if(type == EntryTypeMessage)
		{
			QString query_str = "DELETE FROM kadu_messages WHERE uid_group_id='%1' AND date(receive_time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
		else if(type == EntryTypeStatus)
		{
			QString query_str = "DELETE FROM kadu_status WHERE uid_group_id='%1' AND date(time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
		else if(type == EntryTypeSms)
		{
			QString query_str = "DELETE FROM kadu_sms WHERE uid_group_id='%1' AND date(receive_time) = date('%2');";
			query_str = query_str.arg(uid_group_str).arg(date.toString(Qt::ISODate));
			executeQuery(query_str);
		}
	}
	else
	{
		//rozmowy
		QString query_str = "DELETE FROM kadu_messages WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//statusy
		query_str = "DELETE FROM kadu_status WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//sms'y
		query_str = "DELETE FROM kadu_sms WHERE uid_group_id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
		//i posprz�taj po sobie
		query_str = "DELETE FROM kadu_uid_groups WHERE id='%1';";
		query_str = query_str.arg(uid_group_str);
		executeQuery(query_str);
	}
*/
	kdebugf2();

}

QList<Chat *> HistorySqlStorage::chatsList()
{
	kdebugf();
	QList<Chat *> chats;
	QSqlQuery query(Database);
	QString query_str = "SELECT DISTINCT chat FROM kadu_messages";
	query.prepare(query_str);

	executeQuery(query);
	while (query.next())
	{
		Chat *chat = ChatManager::instance()->byUuid(query.value(0).toString()); 
		if (chat)
			chats.append(chat); 
	}
	kdebugf2();
	return chats;
}

QList<QDate> HistorySqlStorage::datesForChat(Chat *chat)
{
	kdebugf();
	QList<QDate> dates;
	QSqlQuery query(Database);
	QString query_str = "SELECT DISTINCT date(receive_time) as date FROM kadu_messages WHERE chat=:chat";
	query.prepare(query_str);
	query.bindValue(":chat", chat->uuid().toString());
	executeQuery(query);

	while (query.next())
		dates.append(query.value(0).toDate());
	kdebugf2();
	return dates;
}

QList<ChatMessage *> HistorySqlStorage::messages(Chat *chat, QDate date, int limit)
{
	kdebugf();
	QList<ChatMessage *> messages;
	QString query_str, limit_str, date_query_str = "";
	QSqlQuery query(Database);
	if (!date.isNull())
		date_query_str = " AND date(receive_time) = date(:date) ";
	if (limit != 0)
		limit_str = " LIMIT :limit ";
	query_str = "SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE chat=:chat" + date_query_str + limit_str + ";";

	query.prepare(query_str);
	query.bindValue(":chat", chat->uuid().toString());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);

	while (query.next())
	{
		bool outgoing = QVariant(query.value(4).toString().split('=').last()).toBool();

		Message msg(chat, outgoing ? Core::instance()->myself() : ContactManager::instance()->byUuid(query.value(0).toString()));
		msg
		    .setContent(query.value(1).toString())
		    .setSendDate(query.value(2).toDateTime())
		    .setReceiveDate(query.value(3).toDateTime());

		ChatMessage* chat_message;
		if (outgoing)
			chat_message = new ChatMessage(msg, TypeSent);
		else
			chat_message = new ChatMessage(msg, TypeReceived);
		messages.append(chat_message);
	}
	kdebugf2();
	return messages;
}

int HistorySqlStorage::messagesCount(Chat *chat, QDate date)
{
	kdebugf();
	int count = 0;
	QString query_str, date_query_str = "";
	QSqlQuery query(Database);
	if(!date.isNull())
		date_query_str = " AND date(receive_time) = date(:date) ";

	query_str = "SELECT COUNT(chat) FROM kadu_messages WHERE chat=:chat" + date_query_str + ";";

	query.prepare(query_str);
	query.bindValue(":chat", chat->uuid().toString());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	executeQuery(query);

	query.next();
	count = query.value(0).toInt();
	kdebugf2();
	return count;
}

QList<QDate> HistorySqlStorage::getAllDates()
{
	kdebugf();

	QList<QDate> result;
/*	QSqlQuery query(Database);
	QString query_str = "SELECT DISTINCT date(send_time) FROM kadu_messages";
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDate date = query.value(0).toDate();
		result.append(date);
	}
	kdebug("%i dates\n", result.count());*/
	kdebugf2();
	return result;
}




QList<ChatMessage*> HistorySqlStorage::historyMessages(const ContactList& uids, QDate date)
{

	kdebugf();
	QList<ChatMessage*> result;
/*	QSqlQuery query(Database);
	QString query_str;
	if(date.isNull())
		query_str = "SELECT uid_group_id, is_outgoing, send_time, receive_time, content FROM kadu_messages WHERE uid_group_id = '%1' ORDER BY receive_time ASC;";
	else
		query_str = "SELECT uid_group_id, is_outgoing, send_time, receive_time, content FROM kadu_messages WHERE uid_group_id = '%1' AND date(receive_time) = date('%2');";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(date.isNull())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		bool outgoing = query.value(1).toBool();
		QDateTime send_time = query.value(2).toDateTime();
		QDateTime receive_time = query.value(3).toDateTime();
		QString message = query.value(4).toString();

		ChatMessage* chat_message;

// 		if(outgoing)
// 			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, receive_time, send_time);
// 		else
// 			chat_message = new ChatMessage((*uids.begin()), ContactList(kadu->myself()), QString::fromUtf8(message), TypeReceived, receive_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i messages\n", result.count());*/
	kdebugf2();
	return result;
}

QList<ChatMessage*> HistorySqlStorage::getStatusEntries(const ContactList& uids, QDate date)
{
	kdebugf();

	QList<ChatMessage*> result;
/*
	QSqlQuery query(Database);
	QString query_str;
	if(!date.isValid())
		query_str = "SELECT status, time, description, ip_port FROM kadu_status WHERE uid_group_id = '%1' ORDER BY time ASC;";
	else
		query_str = "SELECT status, time, description, ip_port FROM kadu_status WHERE uid_group_id = '%1' AND date(time) = date('%2');";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(!date.isValid())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(1).toDateTime();
		QDateTime receive_time = query.value(1).toDateTime();
		QString message = query.value(0).toString();
		UserStatus us = UserStatus();
		us.fromString(query.value(0).toString());
		us.setDescription(query.value(2).toString());
		QStringList address = query.value(3).toString().split(':');
		Contact user = (*uids.begin());
		if(!address.isEmpty())
			if(address.count() == 1)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), 0);
			else if(address.count() == 2)
				user.setAddressAndPort("Gadu", QHostAddress(address.at(0)), address.at(1).toUInt());
		user.setStatus("Gadu", us, true, false);
		ChatMessage* chat_message = new ChatMessage(user, ContactList(kadu->myself()), QString::fromUtf8(message + " : " + query.value(2).toString()), TypeReceived, receive_time, send_time);
		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
*/
	kdebugf2();
	return result;
}

QList<ChatMessage*> HistorySqlStorage::getSmsEntries(const ContactList& uids, QDate date)
{
	kdebugf();
	QList<ChatMessage*> result;
/*
	QSqlQuery query(Database);
	QString query_str;

	if(date.isValid())
		query_str = "SELECT send_time, content FROM kadu_sms WHERE uid_group_id = '%1' ORDER BY send_time ASC;";
	else
		query_str = "SELECT send_time, content FROM kadu_sms WHERE uid_group_id = '%1' AND date(send_time) = date('%2') ORDER BY send_time ASC;";
	QString uid_group_str = findUidGroup(uids);
	if (uid_group_str == "")
	{
		kdebugf2();
		return result;
	}
	if(date.isNull())
		query_str = query_str.arg(uid_group_str);
	else
	{
		QString recv_time_str = date.toString(Qt::ISODate);
		query_str = query_str.arg(uid_group_str).arg(recv_time_str);
	}

	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return result;
	}
	while (query.next())
	{
		QDateTime send_time = query.value(0).toDateTime();
		QString message = query.value(1).toString();
		ChatMessage* chat_message;
			chat_message = new ChatMessage(kadu->myself(), uids, QString::fromUtf8(message), TypeSent, send_time, send_time);

		result.append(chat_message);
	}
	
	kdebug("%i status entries\n", result.count());
*/
	kdebugf2();
	return result;
}


int HistorySqlStorage::getEntriesCount(const QList<ContactList> &uids, HistoryEntryType type)
{
	kdebugf();
	int result = 0;
/*	QSqlQuery query(Database);
	QString query_str;

	foreach(const ContactList uid, uids)
{
	if(type == EntryTypeMessage)
	{

		QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_messages WHERE uid_group_id = '%1';";
			
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
	result +=part;
	}
	else if(type == EntryTypeStatus)
	{
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_status WHERE uid_group_id = '%1';";
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
	result +=part;
	}	
	else if(type == EntryTypeSms)
	{
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_sms WHERE uid_group_id = '%1';";
	QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
		return 0;
	}
	query.next();
	int part = query.value(0).toInt();
	result +=part;
	}	
	else if(type == EntryTypeAll)
	{
		int part = 0;
			QString query_str = "SELECT COUNT(uid_group_id) FROM kadu_messages WHERE uid_group_id = '%1';";
			QString uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		int sub = query.value(0).toInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_status WHERE uid_group_id = '%1';";
			uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		query.next();
		sub = query.value(0).toInt();
		part += sub;

			query_str = "SELECT COUNT(uid_group_id) FROM kadu_sms WHERE uid_group_id = '%1';";
	uid_group_str = findUidGroup(uid);
	if (uid_group_str == "")
	{
		kdebugf2();
		return 0;
	}
	query_str = query_str.arg(uid_group_str);
	kdebug("query: %s\n", query_str.toLocal8Bit().data());
	if (!query.exec(query_str))
		{
			MessageBox::msg(query.lastError().text(), false, "Warning");
			kdebugf2();
			return 0;
		}
		sub = query.value(0).toInt();
		part += sub;
		result +=part;
	}	
}*/
kdebugf2();
return result;
	
}

HistorySearchResult HistorySqlStorage::searchHistory(ContactList users, HistorySearchParameters params)
{
	kdebugf();

	int count = 0;
	QString title, tableName, query_str;
	HistorySearchResult result;
/*	result.users = users;
	result.currentType = params.currentType;
	QSqlQuery query(QSqlDatabase::database("kadu-history"));
	QString usersID = findUidGroup(users);
	if(params.currentType == EntryTypeStatus)
		tableName = DbPrefix + "status";
	else if(params.currentType == EntryTypeSms)
		tableName = DbPrefix + "sms";
	else
		tableName = DbPrefix + "messages";
	if(params.currentType == EntryTypeStatus)
		query_str = "SELECT time, description FROM " + tableName + " WHERE uid_group_id = :uid ";
	else
		query_str = "SELECT receive_time, content FROM " + tableName + " WHERE uid_group_id = :uid ";
	if (!params.fromDate.isNull() && !params.toDate.isNull())
   		query_str += "AND ( date(receive_time) BETWEEN ':fromdate' AND ':todate') ";
	else if (!params.fromDate.isNull() && params.toDate.isNull())
		query_str += "AND ( date(receive_time) > ':fromdate') ";
	else if (params.fromDate.isNull() && !params.toDate.isNull())
		query_str += "AND ( date(receive_time) < ':todate') ";
	
	if(params.currentType == EntryTypeStatus)
		//mo�na by szuka� i po statusie, ale jaki� problem z 'OR ... ' w zapytaniu - brak wynik�w wtedy
		query_str += "AND description LIKE :pattern"; //OR status like :pattern";
	else
		query_str += "AND content LIKE :pattern ";
	query.prepare(query_str);
	if (!params.fromDate.isNull())
		query.bindValue(":fromdate", params.fromDate.toString(Qt::ISODate));
	if (!params.toDate.isNull())
		query.bindValue(":todate", params.toDate.toString(Qt::ISODate));
	query.bindValue(":uid", usersID);
	if(params.wholeWordsSearch)
		query.bindValue(":pattern", "% " + params.pattern + " %");
	else 
		query.bindValue(":pattern", "%" + params.pattern + "%");
	if (!query.exec())
	{
		MessageBox::msg(query.lastError().text(), false, "Warning");
		kdebugf2();
	}
	kdebug("query: %s\n", query.executedQuery().toLocal8Bit().data());
/*
	while(query.next())
	{	
		title = QString::fromUtf8(query.value(1).toString());
		title.truncate(40);
		title += " ...";
		QDate recvDate = query.value(0).toDate();
		HistorySearchDetailsItem d = HistorySearchDetailsItem((*users.begin()).altNick(), title, recvDate, 1);
		if(!result.detailsItems.isEmpty())
		{
			if(recvDate == result.detailsItems.last().date)
				++result.detailsItems.last().length;
			else
				result.detailsItems.append(d);
		}
		else
			result.detailsItems.append(d);
		//++count;
	}
*/
//	result.pattern = params.pattern;
	return result;
	kdebugf2();
}

QString HistorySqlStorage::prepareText(const QString &text)
{
	QString str = text;
	str.replace('\'', "\'\'");
	str.replace('\\', "\\\\");
	return str;
}

void HistorySqlStorage::executeQuery(QSqlQuery query)
{
	kdebugf();
	query.exec();
	kdebug("db query: %s\n", query.executedQuery().toLocal8Bit().data());
	kdebugf2();
}
