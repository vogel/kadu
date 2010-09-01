/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "debug.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "gui/widgets/chat-widget.h"

#include "modules/history/search/history-search-parameters.h"

#include "history-sql-storage.h"
#include <chat/chat-details.h>
#include <chat/chat-details-aggregate.h>

HistorySqlStorage::HistorySqlStorage(QObject *parent) :
		HistoryStorage(parent), DatabaseMutex(QMutex::NonRecursive)
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

	Database.commit();

	QSqlDatabase::removeDatabase("kadu-history");
}

void HistorySqlStorage::crash()
{
	Database.commit();
}

void HistorySqlStorage::initDatabase()
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageDialog::msg(tr("It seems your Qt library does not provide support for selected database.\n Please select another driver in configuration window or install Qt with %1 plugin.").arg("QSQLITE"), false, "32x32/dialog-warning.png");
		return;
	}

	if (QSqlDatabase::contains("kadu-history"))
	{
		if (Database.isOpen())
			Database.close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	QDir historyDir(profilePath("history"));
	if (!historyDir.exists())
		historyDir.mkpath(profilePath("history"));

	Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	Database.setDatabaseName(profilePath("history/history.db"));

	if (!Database.open())
	{
		MessageDialog::msg(Database.lastError().text(), false, "32x32/dialog-warning.png");
		return;
	}

	Database.transaction();

	initTables();
	initIndexes();
}

void HistorySqlStorage::initTables()
{
	if (!Database.tables().contains("kadu_messages"))
		initKaduMessagesTable();
	if (!Database.tables().contains("kadu_statuses"))
		initKaduStatusesTable();
	if (!Database.tables().contains("kadu_sms"))
		initKaduSmsTable();
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

void HistorySqlStorage::initKaduStatusesTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	executeQuery(query);

	query.prepare("PRAGMA synchronous = OFF;");
	executeQuery(query);

	query.prepare(
		"CREATE TABLE kadu_statuses ("
			"contact VARCHAR(255),"
			"status VARCHAR(255),"
			"set_time TIMESTAMP,"
			"description TEXT);"
	);
	executeQuery(query);
}

void HistorySqlStorage::initKaduSmsTable()
{
	QSqlQuery query(Database);

	query.prepare("PRAGMA encoding = \"UTF-8\";");
	executeQuery(query);

	query.prepare("PRAGMA synchronous = OFF;");
	executeQuery(query);

	query.prepare(
		"CREATE TABLE kadu_sms ("
			"receipient VARCHAR(255),"
			"send_time TIMESTAMP,"
			"content TEXT);"
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

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_statuses_contact ON kadu_statuses (contact)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_statuses_contact_time ON kadu_statuses (contact, set_time)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_statuses_contact_time_date ON kadu_statuses (contact, date(set_time))");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_sms_receipient ON kadu_sms (receipient)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_sms_receipient_time ON kadu_sms (receipient, send_time)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_sms_receipient_time_date ON kadu_sms (receipient, date(send_time))");
	executeQuery(query);
}

void HistorySqlStorage::initQueries()
{
	ListChatsQuery = QSqlQuery(Database);
	ListChatsQuery.prepare("SELECT DISTINCT chat FROM kadu_messages");

	AppendMessageQuery = QSqlQuery(Database);
	AppendMessageQuery.prepare("INSERT INTO kadu_messages (chat, sender, send_time, receive_time, content, attributes) VALUES "
			"(:chat, :sender, :send_time, :receive_time, :content, :attributes)");

	AppendStatusQuery = QSqlQuery(Database);
	AppendStatusQuery.prepare("INSERT INTO kadu_statuses (contact, status, set_time, description) VALUES "
			"(:contact, :status, :set_time, :description)");

	AppendSmsQuery = QSqlQuery(Database);
	AppendSmsQuery.prepare("INSERT INTO kadu_sms (receipient, send_time, content) VALUES "
			"(:receipient, :send_time, :content)");
}

QString HistorySqlStorage::chatWhere(Chat chat)
{
	if (!chat)
		return QLatin1String("false");

	ChatDetails *details = chat.details();
	if (!details)
		return QLatin1String("false");

	ChatDetailsAggregate *aggregate = dynamic_cast<ChatDetailsAggregate *>(details);
	if (!aggregate)
		return QString("chat = '%1'").arg(chat.uuid().toString());

	QStringList uuids;
	foreach (Chat aggregatedChat, aggregate->chats())
		uuids.append(QString("'%1'").arg(aggregatedChat.uuid().toString()));

	return QString("chat IN (%1)").arg(uuids.join(QLatin1String(", ")));
}

QString HistorySqlStorage::buddyContactsWhere(Buddy buddy)
{
	if (!buddy || buddy.contacts().isEmpty())
		return  QLatin1String("false");

	QStringList uuids;
	foreach (Contact contact, buddy.contacts())
		uuids.append(QString("'%1'").arg(contact.uuid().toString()));

	return QString("contact IN (%1)").arg(uuids.join(QLatin1String(", ")));
}

void HistorySqlStorage::sync()
{
	DatabaseMutex.lock();

	Database.commit();
	Database.transaction();

	DatabaseMutex.unlock();
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

	QString outgoing = (message.messageSender().ownerBuddy() == Core::instance()->myself())
			? "1"
			: "0";

	AppendMessageQuery.bindValue(":chat", message.messageChat().uuid().toString());
	AppendMessageQuery.bindValue(":sender", message.messageSender().uuid().toString());
	AppendMessageQuery.bindValue(":send_time", message.sendDate());
	AppendMessageQuery.bindValue(":receive_time", message.receiveDate());
	AppendMessageQuery.bindValue(":content", message.content());
	AppendMessageQuery.bindValue(":attributes", QString("outgoing=%1").arg(outgoing));

	executeQuery(AppendMessageQuery);

	DatabaseMutex.unlock();

	kdebugf2();
}

void HistorySqlStorage::appendStatus(Contact contact, Status status, QDateTime time)
{
	kdebugf();

	DatabaseMutex.lock();

	AppendStatusQuery.bindValue(":contact", contact.uuid().toString());
	AppendStatusQuery.bindValue(":status", status.type());
	AppendStatusQuery.bindValue(":set_time", time);
	AppendStatusQuery.bindValue(":description", status.description());

	executeQuery(AppendStatusQuery);

	DatabaseMutex.unlock();

	kdebugf2();
}

void HistorySqlStorage::appendSms(const QString &recipient, const QString &content, QDateTime time)
{
	kdebugf();

	DatabaseMutex.lock();

	AppendSmsQuery.bindValue(":contact", recipient);
	AppendSmsQuery.bindValue(":send_time", time);
	AppendSmsQuery.bindValue(":content", content);

	executeQuery(AppendSmsQuery);

	DatabaseMutex.unlock();

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(Chat chat)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages WHERE " + chatWhere(chat);
	query.prepare(queryString);

	executeQuery(query);

	DatabaseMutex.unlock();
}

void HistorySqlStorage::deleteHistory(Buddy buddy)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);

	foreach (Contact contact, buddy.contacts())
	{
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact), false);
		if (chat)
		{
			QString queryString = "DELETE FROM kadu_messages WHERE " + chatWhere(chat);
			query.prepare(queryString);
			executeQuery(query);
		}
	}

	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	query.prepare(queryString);
	executeQuery(query);

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

	if (!chat)
		return QList<QDate>();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT date(receive_time) as date FROM kadu_messages WHERE " + chatWhere(chat);

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

	QSqlQuery query(Database);
	QString queryString = "SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date(receive_time) = date(:date)";
	queryString += " ORDER BY receive_time";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QList<Message> messages;
	query.prepare(queryString);

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

	QSqlQuery query(Database);
	QString queryString = "SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat) +
			" AND date(receive_time) >= date(:date) ORDER BY receive_time";
	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);

	messages = messagesFromQuery(chat, query);

	DatabaseMutex.unlock();

	return messages;
}

QList<Message> HistorySqlStorage::messagesBackTo(Chat chat, QDateTime datetime, int limit)
{
	DatabaseMutex.lock();

	QList<Message> result;

	QSqlQuery query(Database);
	QString queryString = "SELECT sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat) +
			" AND datetime(receive_time) >= datetime(:date) ORDER BY receive_time DESC LIMIT :limit";
	query.prepare(queryString);

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

	QSqlQuery query(Database);
	QString queryString = "SELECT COUNT(chat) FROM kadu_messages WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date(receive_time) = date(:date)";
	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
	query.next();

	DatabaseMutex.unlock();

	return query.value(0).toInt();
}

QList<QString> HistorySqlStorage::smsRecipientsList(HistorySearchParameters search)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT receipient FROM kadu_sms WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND date(send_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(send_time) <= date(:toDate)";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":content", QLatin1String("%") + search.query() + "%");
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QString> recipients;

	executeQuery(query);
	while (query.next())
		recipients.append(query.value(0).toString());

	DatabaseMutex.unlock();

	return recipients;
}

QList<QDate> HistorySqlStorage::datesForSmsRecipient(const QString &recipient, HistorySearchParameters search)
{
	kdebugf();

	if (recipient.isEmpty())
		return QList<QDate>();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT date(send_time) as date FROM kadu_sms WHERE receipient = :receipient";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND date(send_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(send_time) <= date(:toDate)";

	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
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

QList<QString> HistorySqlStorage::sms(const QString &recipient, QDate date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT content, send_time FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND date(send_time) = date(:date)";
	queryString += " ORDER BY send_time";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QList<Message> messages;
	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);

	QList<QString> result;

	while (query.next())
		result.append(query.value(0).toString());

	DatabaseMutex.unlock();

	return result;
}

int HistorySqlStorage::smsCount(const QString &recipient, QDate date)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT COUNT(receipient) FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND date(send_time) = date(:date)";
	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
	query.next();

	DatabaseMutex.unlock();

	return query.value(0).toInt();
}

QList<Buddy> HistorySqlStorage::statusBuddiesList(HistorySearchParameters search)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT contact FROM kadu_statuses WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND description LIKE :description";
	if (search.fromDate().isValid())
		queryString += " AND date(set_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(set_time) <= date(:toDate)";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":description", QLatin1String("%") + search.query() + "%");
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<Buddy> buddies;
	QSet<Contact> usedContacts;

	executeQuery(query);
	while (query.next())
	{
		Contact contact = ContactManager::instance()->byUuid(query.value(0).toString());
		if (contact && !usedContacts.contains(contact))
		{
			Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
			buddies.append(buddy);
			foreach (Contact contact, buddy.contacts())
				usedContacts.insert(contact);
		}
	}

	DatabaseMutex.unlock();

	return buddies;
}

QList<QDate> HistorySqlStorage::datesForStatusBuddy(Buddy buddy, HistorySearchParameters search)
{
	kdebugf();

	if (!buddy)
		return QList<QDate>();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT date(set_time) as date FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);

	if (!search.query().isEmpty())
		queryString += " AND description LIKE :description";
	if (search.fromDate().isValid())
		queryString += " AND date(set_time) >= date(:fromDate)";
	if (search.toDate().isValid())
		queryString += " AND date(set_time) <= date(:toDate)";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":description", QLatin1String("%") + search.query() + "%");
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

QList<Status> HistorySqlStorage::statuses(Buddy buddy, QDate date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT contact, status, description, set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND date(set_time) = date(:date)";
	queryString += " ORDER BY set_time";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QList<Status> statuses;
	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);

	executeQuery(query);
	statuses = statusesFromQuery(query);

	DatabaseMutex.unlock();

	return statuses;
}

int HistorySqlStorage::statusBuddyCount(Buddy buddy, QDate date)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT COUNT(contact) FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND date(set_time) = date(:date)";
	query.prepare(queryString);

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

// 	QDateTime before = QDateTime::currentDateTime();
	query.exec();
// 	QDateTime after = QDateTime::currentDateTime();

// 	printf("[%s]\n[%d.%d]-[%d.%d]/%d.%d\n", qPrintable(query.executedQuery()),
// 			before.toTime_t(), before.time().msec(),
// 			after.toTime_t(), after.time().msec(),
// 			after.toTime_t() - before.toTime_t(),
// 			after.time().msec() - before.time().msec());


	kdebug("db query: %s\n", qPrintable(query.executedQuery()));
}


QList<Message> HistorySqlStorage::messagesFromQuery(Chat chat, QSqlQuery query)
{
	QList<Message> messages;

	while (query.next())
	{
		bool outgoing = QVariant(query.value(4).toString().split('=').last()).toBool();

		Message::Type type = outgoing ? Message::TypeSent : Message::TypeReceived;

		// ignore non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(0).toString());
		if (sender.isNull())
			continue;

		Message message = Message::create();
		message.setMessageChat(chat);
		message.setType(type);
		message.setMessageSender(sender);
		message.setContent(query.value(1).toString());
		message.setSendDate(query.value(2).toDateTime());
		message.setReceiveDate(query.value(3).toDateTime());
		message.setStatus(outgoing ? Message::StatusDelivered : Message::StatusReceived);

		messages.append(message);
	}

	return messages;
}

QList<Status> HistorySqlStorage::statusesFromQuery(QSqlQuery query)
{
	QList<Status> statuses;

	while (query.next())
	{
		// ignore non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(0).toString());
		if (sender.isNull())
			continue;

		Status status;
		status.setType(query.value(1).toString());
		status.setDescription(query.value(2).toString());

		statuses.append(status);
	}

	return statuses;
}

void HistorySqlStorage::convertSenderToContact()
{
	QList<Chat> allChats = ChatManager::instance()->allItems();
	QList<Buddy> allBuddies = BuddyManager::instance()->items();

	foreach (Chat chat, allChats)
	{
		foreach (Buddy buddy, allBuddies)
		{
			QList<Contact> contacts = buddy.contacts(chat.chatAccount());
			if (contacts.isEmpty())
				continue;

			Contact contact = contacts[0];

			QSqlQuery import = QSqlQuery(Database);
			import.prepare("UPDATE kadu_messages SET sender=:sender WHERE sender=:old_sender AND chat=:old_chat");
			import.bindValue(":old_sender", buddy.uuid().toString());
			import.bindValue(":old_chat", chat.uuid().toString());
			import.bindValue(":sender", contact.uuid().toString());
			import.exec();
		}
		if (!chat.chatAccount())
			continue;

		Contact sender = chat.chatAccount().accountContact();
		QSqlQuery import = QSqlQuery(Database);
		import.prepare("UPDATE kadu_messages SET sender=:sender WHERE attributes=:old_attr AND chat=:old_chat");
		import.bindValue(":old_attr", "outgoing=1");
		import.bindValue(":old_chat", chat.uuid().toString());
		import.bindValue(":sender", sender.uuid().toString());
		import.exec();
	}

	MessageDialog::msg("All teh werk dun!", false, "32x32/dialog-warning.png");
}
