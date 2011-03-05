/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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
#include "chat/chat-details.h"
#include "chat/chat-details-aggregate.h"
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

#include "modules/history/history.h"
#include "modules/history/search/history-search-parameters.h"
#include "modules/history/timed-status.h"

#include "history-sql-storage.h"

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
		MessageDialog::show("dialog-warning", tr("Kadu"),
				tr("It seems your Qt library does not provide support for selected database.\n "
				   "Please select another driver in configuration window or install Qt with %1 plugin.").arg("QSQLITE"));
		History::instance()->unregisterStorage(this);
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
		MessageDialog::show("dialog-warning", tr("Kadu"), Database.lastError().text());
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

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat_receive_time_rowid ON kadu_messages (chat, receive_time, rowid)");
	executeQuery(query);

	query.prepare("DROP INDEX IF EXISTS kadu_messages_chat_receive_time");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_messages_chat_receive_time_date ON kadu_messages (chat, date(receive_time))");
	executeQuery(query);

	query.prepare("DROP INDEX IF EXISTS kadu_messages_chat_receive_time_send_time");
	executeQuery(query);

	query.prepare("DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date_send_time");
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

QString HistorySqlStorage::chatWhere(const Chat &chat)
{
	if (!chat)
		return QLatin1String("false");

	ChatDetails *details = chat.details();
	if (!details)
		return QLatin1String("false");

	ChatDetailsAggregate *aggregate = qobject_cast<ChatDetailsAggregate *>(details);
	if (!aggregate)
		return QString("chat = '%1'").arg(chat.uuid().toString());

	QStringList uuids;
	foreach (const Chat &aggregatedChat, aggregate->chats())
		uuids.append(QString("'%1'").arg(aggregatedChat.uuid().toString()));

	return QString("chat IN (%1)").arg(uuids.join(QLatin1String(", ")));
}

QString HistorySqlStorage::buddyContactsWhere(const Buddy &buddy)
{
	if (!buddy || buddy.contacts().isEmpty())
		return  QLatin1String("false");

	QStringList uuids;
	foreach (const Contact &contact, buddy.contacts())
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

	QString outgoing = (message.type() == Message::TypeSent)
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

void HistorySqlStorage::appendStatus(const Contact &contact, const Status &status, const QDateTime &time)
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

void HistorySqlStorage::appendSms(const QString &recipient, const QString &content, const QDateTime &time)
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

void HistorySqlStorage::clearChatHistory(const Chat &chat, const QDate &date)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date(receive_time) = date(:date)";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);

	DatabaseMutex.unlock();
}

void HistorySqlStorage::clearStatusHistory(const Buddy &buddy, const QDate &date)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND date(set_time) = date(:date)";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);

	DatabaseMutex.unlock();
}

void HistorySqlStorage::clearSmsHistory(const QString &recipient, const QDate &date)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND date(send_time) = date(:date)";

	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);

	DatabaseMutex.unlock();
}

void HistorySqlStorage::deleteHistory(const Buddy &buddy)
{
	DatabaseMutex.lock();

	QSqlQuery query(Database);

	foreach (const Contact &contact, buddy.contacts())
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

QList<Chat> HistorySqlStorage::chats(const HistorySearchParameters &search)
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
		query.bindValue(":content", QLatin1String("%") + search.query() + '%');
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<Chat> chats;

	executeQuery(query);
#if (QT_VERSION >= 0x040700)
	chats.reserve(query.size());
#endif

	while (query.next())
	{
		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat)
			chats.append(chat);
	}

	DatabaseMutex.unlock();

	return chats;
}

QList<QDate> HistorySqlStorage::chatDates(const Chat &chat, const HistorySearchParameters &search)
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
		query.bindValue(":content", QLatin1String("%") + search.query() + '%');
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QDate> dates;
	executeQuery(query);
#if (QT_VERSION >= 0x040700)
	dates.reserve(query.size());
#endif

	while (query.next())
	{
		QDate date = query.value(0).toDate();
		if (date.isValid())
			dates.append(date);
	}

	DatabaseMutex.unlock();

	return dates;
}

QList<Message> HistorySqlStorage::messages(const Chat &chat, const QDate &date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT chat, sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date(receive_time) = date(:date)";
	queryString += " ORDER BY receive_time ASC, rowid ASC";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QList<Message> messages;
	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);
	messages = messagesFromQuery(query);

	DatabaseMutex.unlock();

	return messages;
}

QList<Message> HistorySqlStorage::messagesSince(const Chat &chat, const QDate &date)
{
	kdebugf();

	DatabaseMutex.lock();

	QList<Message> messages;
	if (date.isNull())
		return messages;

	QSqlQuery query(Database);
	QString queryString = "SELECT chat, sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat) +
			" AND date(receive_time) >= date(:date) ORDER BY receive_time ASC, rowid ASC";
	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);

	messages = messagesFromQuery(query);

	DatabaseMutex.unlock();

	return messages;
}

QList<Message> HistorySqlStorage::messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit)
{
	DatabaseMutex.lock();

	QList<Message> result;

	QSqlQuery query(Database);
	// we want last *limit* messages, so we have to invert sorting here
	// it is reverted back manually below
	QString queryString = "SELECT chat, sender, content, send_time, receive_time, attributes FROM kadu_messages WHERE " + chatWhere(chat) +
			" AND datetime(receive_time) >= datetime(:date) ORDER BY receive_time DESC, rowid DESC LIMIT :limit";
	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", datetime.toString(Qt::ISODate));
	query.bindValue(":limit", limit);

	executeQuery(query);

	result = messagesFromQuery(query);

	DatabaseMutex.unlock();

	// se comment above
	QList<Message> inverted;
#if (QT_VERSION >= 0x040700)
	inverted.reserve(result.size());
#endif
	for (int i = result.size() - 1; i >= 0; --i)
		inverted.append(result.at(i));
	return inverted;
}

int HistorySqlStorage::messagesCount(const Chat &chat, const QDate &date)
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

QList<QString> HistorySqlStorage::smsRecipientsList(const HistorySearchParameters &search)
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
		query.bindValue(":content", QLatin1String("%") + search.query() + '%');
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QString> recipients;

	executeQuery(query);
#if (QT_VERSION >= 0x040700)
	recipients.reserve(query.size());
#endif

	while (query.next())
		recipients.append(query.value(0).toString());

	DatabaseMutex.unlock();

	return recipients;
}

QList<QDate> HistorySqlStorage::datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search)
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
		query.bindValue(":content", QLatin1String("%") + search.query() + '%');
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QDate> dates;
	executeQuery(query);
#if (QT_VERSION >= 0x040700)
	dates.reserve(query.size());
#endif

	while (query.next())
	{
		QDate date = query.value(0).toDate();
		if (date.isValid())
			dates.append(date);
	}

	DatabaseMutex.unlock();

	return dates;
}

QList<Message> HistorySqlStorage::sms(const QString &recipient, const QDate &date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT content, send_time FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND date(send_time) = date(:date)";
	queryString += " ORDER BY send_time ASC";
	if (0 != limit)
		queryString += " LIMIT :limit";

	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);

	QList<Message> result = smsFromQuery(query);

	DatabaseMutex.unlock();

	return result;
}

int HistorySqlStorage::smsCount(const QString &recipient, const QDate &date)
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

QList<Buddy> HistorySqlStorage::statusBuddiesList(const HistorySearchParameters &search)
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
		query.bindValue(":description", QLatin1String("%") + search.query() + '%');
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
			foreach (const Contact &contact, buddy.contacts())
				usedContacts.insert(contact);
		}
	}

	DatabaseMutex.unlock();

	return buddies;
}

QList<QDate> HistorySqlStorage::datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search)
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
		query.bindValue(":description", QLatin1String("%") + search.query() + '%');
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QDate> dates;

	executeQuery(query);
#if (QT_VERSION >= 0x040700)
	dates.reserve(query.size());
#endif

	while (query.next())
	{
		QDate date = query.value(0).toDate();
		if (date.isValid())
			dates.append(date);
	}

	DatabaseMutex.unlock();

	return dates;
}

QList<TimedStatus> HistorySqlStorage::statuses(const Buddy &buddy, const QDate &date, int limit)
{
	kdebugf();

	DatabaseMutex.lock();

	QSqlQuery query(Database);
	QString queryString = "SELECT contact, status, description, set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND date(set_time) = date(:date)";
	queryString += " ORDER BY set_time ASC";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QList<TimedStatus> statuses;
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

int HistorySqlStorage::statusBuddyCount(const Buddy &buddy, const QDate &date)
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


	kdebugm(KDEBUG_INFO, "db query: %s\n", qPrintable(query.executedQuery()));
}


QList<Message> HistorySqlStorage::messagesFromQuery(QSqlQuery query)
{
	QList<Message> messages;
#if (QT_VERSION >= 0x040700)
	messages.reserve(query.size());
#endif
	while (query.next())
	{
		bool outgoing = QVariant(query.value(5).toString().split('=').last()).toBool();

		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat.isNull())
			continue;

		Message::Type type = outgoing ? Message::TypeSent : Message::TypeReceived;

		// ignore non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(1).toString());
		if (sender.isNull())
			continue;

		Message message = Message::create();
		message.setMessageChat(chat);
		message.setType(type);
		message.setMessageSender(sender);
		message.setContent(query.value(2).toString());
		message.setSendDate(query.value(3).toDateTime());
		message.setReceiveDate(query.value(4).toDateTime());
		message.setStatus(outgoing ? Message::StatusDelivered : Message::StatusReceived);

		messages.append(message);
	}

	return messages;
}

QList<TimedStatus> HistorySqlStorage::statusesFromQuery(QSqlQuery query)
{
	QList<TimedStatus> statuses;
#if (QT_VERSION >= 0x040700)
	statuses.reserve(query.size());
#endif
	while (query.next())
	{
		// ignore non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(0).toString());
		if (sender.isNull())
			continue;

		Status status;
		status.setType(query.value(1).toString());
		status.setDescription(query.value(2).toString());

		TimedStatus timedStatus(status, query.value(3).toDateTime());

		statuses.append(timedStatus);
	}

	return statuses;
}

QList<Message> HistorySqlStorage::smsFromQuery(QSqlQuery query)
{
	QList<Message> messages;
#if (QT_VERSION >= 0x040700)
	messages.reserve(query.size());
#endif

	while (query.next())
	{
		Message message = Message::create();
		message.setStatus(Message::StatusSent);
		message.setType(Message::TypeSystem);
		message.setReceiveDate(query.value(1).toDateTime());
		message.setSendDate(query.value(1).toDateTime());
		message.setContent(query.value(0).toString());

		messages.append(message);
	}

	return messages;
}
