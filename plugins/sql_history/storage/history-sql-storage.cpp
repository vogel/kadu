/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QDir>
#include <QtCore/QMutexLocker>
#include <QtGui/QTextDocument>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/widgets/chat-widget.h"
#include "gui/windows/message-dialog.h"
#include "message/formatted-message.h"
#include "message/message.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "status/status-type-manager.h"
#include "debug.h"

#include "plugins/history/history.h"
#include "plugins/history/model/dates-model-item.h"
#include "plugins/history/search/history-search-parameters.h"
#include "plugins/history/timed-status.h"

#include "history-sql-storage.h"

HistorySqlStorage::HistorySqlStorage(QObject *parent) :
		HistoryStorage(parent), DatabaseMutex(QMutex::NonRecursive)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	initDatabase();
	initQueries();
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

	Database.commit();
}

void HistorySqlStorage::initDatabase()
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"),
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
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), Database.lastError().text());
		return;
	}

	import010History();

	Database.transaction();

	initTables();
	initIndexes();
}

void HistorySqlStorage::import010History()
{
	if (Database.tables().contains("kadu_chats")
	        && Database.tables().contains("kadu_dates")
	        && Database.tables().contains("kadu_contacts")
	        && Database.tables().contains("kadu_message_contents"))
		return;

	MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("We have to update your chats history to the latest version. Please be patient, it will take few minutes."));

	QSqlQuery query(Database);
	Database.transaction();

	QStringList queries;

	queries
	        << "PRAGMA foreign_keys = ON;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_rowid;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_send_time;"
	        << "DROP INDEX IF EXISTS kadu_messages_chat_receive_time_date_send_time;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact_time;"
	        << "DROP INDEX IF EXISTS kadu_statuses_contact_time_date;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient_time;"
	        << "DROP INDEX IF EXISTS kadu_sms_receipient_time_date;"

	        << "CREATE TABLE kadu_chats (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
	        << "CREATE TABLE kadu_contacts (id INTEGER PRIMARY KEY AUTOINCREMENT, uuid VARCHAR(16));"
	        << "CREATE TABLE kadu_message_contents (id INTEGER PRIMARY KEY AUTOINCREMENT, content TEXT, attributes VARCHAR(25));"
	        << "CREATE TABLE kadu_dates (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER);"

	        << "ALTER TABLE kadu_messages RENAME TO kadu_messages_old;"

	        << "INSERT INTO kadu_chats (uuid) SELECT DISTINCT chat FROM kadu_messages_old;"
	        << "INSERT INTO kadu_contacts (uuid) SELECT DISTINCT sender FROM kadu_messages_old;"
	        << "INSERT INTO kadu_dates (date) SELECT DISTINCT REPLACE(substr(receive_time,0,11), '-', '') FROM kadu_messages_old;"
	        << "INSERT INTO kadu_message_contents (content) SELECT DISTINCT content FROM kadu_messages_old;"

	        << "CREATE TABLE kadu_messages ("
	           "chat_id INTEGER,"
	           "contact_id INTEGER,"
	           "date_id INTEGER,"
	           "send_time TIMESTAMP,"
	           "receive_time TIMESTAMP,"
	           "content_id INTEGER,"
	           "is_outgoing BOOL, "
	           "FOREIGN KEY (chat_id) REFERENCES kadu_chats(id), FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id), "
	           "FOREIGN KEY (date_id) REFERENCES kadu_dates(id), FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"

	        << "INSERT INTO kadu_messages ("
	           "chat_id, contact_id, date_id, send_time, receive_time, content_id, is_outgoing) "
	           "SELECT (SELECT id FROM kadu_chats WHERE uuid=old.chat LIMIT 1), (SELECT id FROM kadu_contacts WHERE uuid=old.sender LIMIT 1), "
	           "(SELECT id FROM kadu_dates WHERE date = REPLACE(substr(old.receive_time,0,11), '-', '')), send_time, receive_time, "
	           "(SELECT id FROM kadu_message_contents WHERE content=old.content LIMIT 1), substr(attributes, 10, 1) FROM kadu_messages_old old;"

	        << "DROP TABLE kadu_messages_old;";

	foreach (const QString &queryString, queries)
	{
		query.prepare(queryString);
		executeQuery(query);
	}

	Database.commit();

	query.prepare("VACUUM;");
	executeQuery(query);

	MessageDialog::show(KaduIcon("dialog-ok"), tr("Kadu"), tr("Chats history was successfuly updated."));
}

void HistorySqlStorage::initTables()
{
	if (!Database.tables().contains("kadu_messages") && !Database.tables().contains("kadu_chats"))
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

	query.prepare("PRAGMA foreign_keys = ON;");
	executeQuery(query);

	query.prepare(
			"CREATE TABLE kadu_chats ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	executeQuery(query);

	query.prepare(
			"CREATE TABLE kadu_contacts ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"uuid VARCHAR(16));"
	);
	executeQuery(query);

	query.prepare(
			"CREATE TABLE kadu_dates ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"date VARCHAR(8));"
	);
	executeQuery(query);

	query.prepare(
			"CREATE TABLE kadu_message_contents ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"content TEXT,"
			"attributes VARCHAR(25));"
	);
	executeQuery(query);

	query.prepare(
			"CREATE TABLE kadu_messages ("
			"chat_id INTEGER,"
			"contact_id INTEGER,"
			"date_id INTEGER,"
			"send_time TIMESTAMP,"
			"receive_time TIMESTAMP,"
			"content_id INTEGER,"
			"is_outgoing BOOL,"
			"FOREIGN KEY (chat_id) REFERENCES kadu_chats(id),"
			"FOREIGN KEY (contact_id) REFERENCES kadu_contacts(id),"
			"FOREIGN KEY (date_id) REFERENCES kadu_dates(id),"
			"FOREIGN KEY (content_id) REFERENCES kadu_message_contents(id));"
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

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_pk ON kadu_chats (id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_chat_uuid ON kadu_chats (uuid)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_pk ON kadu_contacts (id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_contact_uuid ON kadu_contacts (uuid)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_pk ON kadu_dates (id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_dates_val ON kadu_dates (date)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_content_pk ON kadu_message_contents (id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_chat ON kadu_messages (chat_id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_contact ON kadu_messages (contact_id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_date ON kadu_messages (date_id)");
	executeQuery(query);

	query.prepare("CREATE INDEX IF NOT EXISTS kadu_msg_content ON kadu_messages (content_id)");
	executeQuery(query);
}

void HistorySqlStorage::initQueries()
{
	ListChatsQuery = QSqlQuery(Database);
	ListChatsQuery.prepare("SELECT uuid FROM kadu_chats");

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
		return QString("chat.uuid = '%1'").arg(chat.uuid().toString());

	QStringList uuids;
	foreach (const Chat &aggregatedChat, aggregate->chats())
		uuids.append(QString("'%1'").arg(aggregatedChat.uuid().toString()));

	return QString("chat.uuid IN (%1)").arg(uuids.join(QLatin1String(", ")));
}

QString HistorySqlStorage::buddyContactsWhere(const Buddy &buddy)
{
	if (!buddy || buddy.contacts().isEmpty())
		return  QLatin1String("false");

	QStringList uuids;
	foreach (const Contact &contact, buddy.contacts())
		uuids.append(QString("'%1'").arg(contact.uuid().toString()));

	return QString("con.uuid IN (%1)").arg(uuids.join(QLatin1String(", ")));
}

void HistorySqlStorage::sync()
{
	QMutexLocker locker(&DatabaseMutex);

	Database.commit();
	Database.transaction();
}

void HistorySqlStorage::messageReceived(const Message &message)
{
	appendMessage(message);
}

void HistorySqlStorage::messageSent(const Message &message)
{
	appendMessage(message);
}

int HistorySqlStorage::findOrCreateChat(const Chat &chat)
{
	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_chats WHERE uuid=:uuid";

	query.prepare(queryString);
	query.bindValue(":uuid", chat.uuid().toString());

	int chatId = -1;

	executeQuery(query);

	if (query.size() > 0)
	{
		query.next();
		chatId = query.value(0).toInt();
	}
	else
	{
		QSqlQuery query(Database);
		QString queryString = "INSERT INTO kadu_chats (uuid) VALUES (:uuid)";

		query.prepare(queryString);
		query.bindValue(":uuid", chat.uuid().toString());

		executeQuery(query);
		chatId = query.lastInsertId().toInt();
	}

	return chatId;
}

int HistorySqlStorage::findOrCreateContact(const Contact &contact)
{
	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_contacts WHERE uuid=:uuid";

	query.prepare(queryString);
	query.bindValue(":uuid", contact.uuid().toString());

	int contactId = -1;

	executeQuery(query);

	if (query.size() > 0)
	{
		query.next();
		contactId = query.value(0).toInt();
	}
	else
	{
		QSqlQuery query(Database);
		QString queryString = "INSERT INTO kadu_contacts (uuid) VALUES (:uuid)";

		query.prepare(queryString);
		query.bindValue(":uuid", contact.uuid().toString());

		executeQuery(query);
		contactId = query.lastInsertId().toInt();
	}

	return contactId;
}

int HistorySqlStorage::findOrCreateDate(const QDate &date)
{
	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_dates WHERE date=:date";

	query.prepare(queryString);
	query.bindValue(":date", date.toString("yyyyMMdd"));

	int dateId = -1;

	executeQuery(query);

	if (query.size() > 0)
	{
	  query.next();
	  dateId = query.value(0).toInt();
	}
	else
	{
	  QSqlQuery query(Database);
	  QString queryString = "INSERT INTO kadu_dates (date) VALUES (:date)";

	  query.prepare(queryString);
	  query.bindValue(":date", date.toString("yyyyMMdd"));

	  executeQuery(query);
	  dateId = query.lastInsertId().toInt();
	}

	return dateId;
}

int HistorySqlStorage::saveMessageContent(const Message& message)
{
	QSqlQuery saveMessageQuery = QSqlQuery(Database);
	saveMessageQuery.prepare("INSERT INTO kadu_message_contents (content) VALUES (:content)");

	saveMessageQuery.bindValue(":content", message.content());

	executeQuery(saveMessageQuery);
	int contentId = saveMessageQuery.lastInsertId().toInt();

	saveMessageQuery.finish();

	return contentId;
}

void HistorySqlStorage::appendMessage(const Message &message)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	int outgoing = (message.type() == MessageTypeSent)
			? 1
			: 0;

	AppendMessageQuery = QSqlQuery(Database);
	AppendMessageQuery.prepare("INSERT INTO kadu_messages (chat_id, contact_id, send_time, receive_time, date_id, is_outgoing, content_id) VALUES "
			"(:chat_id, :contact_id, :send_time, :receive_time, :date_id, :is_outgoing, :content_id)");

	AppendMessageQuery.bindValue(":chat_id", findOrCreateChat(message.messageChat()));
	AppendMessageQuery.bindValue(":contact_id", findOrCreateContact(message.messageSender()));
	AppendMessageQuery.bindValue(":send_time", message.sendDate());
	AppendMessageQuery.bindValue(":receive_time", message.receiveDate());
	AppendMessageQuery.bindValue(":date_id", findOrCreateDate(message.receiveDate().date()));
	AppendMessageQuery.bindValue(":is_outgoing", outgoing);
	AppendMessageQuery.bindValue(":content_id", saveMessageContent(message));

	executeQuery(AppendMessageQuery);

	AppendMessageQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::appendStatus(const Contact &contact, const Status &status, const QDateTime &time)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	AppendStatusQuery.bindValue(":contact", contact.uuid().toString());
	AppendStatusQuery.bindValue(":status", status.type());
	AppendStatusQuery.bindValue(":set_time", time);
	AppendStatusQuery.bindValue(":description", status.description());

	executeQuery(AppendStatusQuery);

	AppendStatusQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::appendSms(const QString &recipient, const QString &content, const QDateTime &time)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	AppendSmsQuery.bindValue(":contact", recipient);
	AppendSmsQuery.bindValue(":send_time", time);
	AppendSmsQuery.bindValue(":content", content);

	executeQuery(AppendSmsQuery);

	AppendSmsQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(const Chat &chat, const QDate &date)
{
	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date = :date";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);
}

void HistorySqlStorage::clearStatusHistory(const Buddy &buddy, const QDate &date)
{
	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND substr(set_time,0,11) = :date";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::clearSmsHistory(const QString &recipient, const QDate &date)
{
	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND substr(send_time,0,11) = :date";

	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::deleteHistory(const Buddy &buddy)
{
	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);

	foreach (const Contact &contact, buddy.contacts())
	{
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact), false);
		if (chat)
		{
			QString queryString = "DELETE FROM kadu_messages LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) WHERE " + chatWhere(chat);
			query.prepare(queryString);
			executeQuery(query);
		}
	}

	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	query.prepare(queryString);
	executeQuery(query);
}

QVector<Chat> HistorySqlStorage::chats(const HistorySearchParameters &search)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QString joins = !search.query().isEmpty() || search.fromDate().isValid() || search.toDate().isValid()
		? "LEFT JOIN kadu_messages km ON (kadu_chats.id=km.chat_id) LEFT JOIN kadu_dates kd ON (kd.id=km.date_id) LEFT JOIN kadu_message_contents ON (kadu_message_contents.id=kadu_messages.content_id) "
		: "";

	QSqlQuery query(Database);
	QString queryString = "SELECT uuid FROM kadu_chats " + joins + "WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND date >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND date <= :toDate";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":content", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate().toString("yyyyMMdd"));
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate().toString("yyyyMMdd"));

	QVector<Chat> chats;

	executeQuery(query);

	while (query.next())
	{
		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat)
			chats.append(chat);
	}

	return chats;
}

QVector<DatesModelItem> HistorySqlStorage::chatDates(const Chat &chat, const HistorySearchParameters &search)
{
	kdebugf();

	if (!chat)
		return QVector<DatesModelItem>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), date, date_id, content FROM";
	queryString += " (SELECT km.rowid, date, date_id, content FROM kadu_messages km "
		"LEFT JOIN kadu_message_contents kmc ON (km.content_id=kmc.id) "
		"LEFT JOIN kadu_dates d ON (km.date_id=d.id) "
		"LEFT JOIN kadu_chats chat ON (km.chat_id=chat.id) WHERE " + chatWhere(chat);
	if (!search.query().isEmpty())
	  queryString += " AND kmc.content LIKE :content";
	if (search.fromDate().isValid())
	  queryString += " AND date >= :fromDate";
	if (search.toDate().isValid())
	  queryString += " AND date <= :toDate";
	queryString += " ORDER BY date_id DESC, km.rowid DESC )";
	queryString += " GROUP BY date_id";
	queryString += " ORDER BY date_id ASC, rowid ASC";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":content", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate().toString("yyyyMMdd"));
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate().toString("yyyyMMdd"));

	QVector<DatesModelItem> dates;
	executeQuery(query);

	int count;
	QString message;
	QDate date;
	while (query.next())
	{
		count = query.value(0).toInt();
		QString dateString = query.value(1).toString();
		date = QDate::fromString(dateString, "yyyyMMdd");
		if (!date.isValid())
			continue;

		message = query.value(2).toString();
		if (message.isEmpty())
			continue;

		// TODO: this should be done in different place
		QTextDocument document;
		document.setHtml(message);
		FormattedMessage formatted = FormattedMessage::parse(&document);
		QString title = formatted.toPlain();

		if (title.length() > 20)
		{
			title.truncate(20);
			title += " ...";
		}

		dates.append(DatesModelItem(date, count, title));
	}

	return dates;
}

QVector<Message> HistorySqlStorage::messages(const Chat &chat, const QDate &date, int limit)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT chat.uuid, con.uuid, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) "
			"LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) "
			"LEFT JOIN kadu_contacts con ON (kadu_messages.contact_id=con.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE " + chatWhere(chat);
	if (!date.isNull())
		queryString += " AND date = :date";
	queryString += " ORDER BY kadu_messages.date_id ASC, kadu_messages.rowid ASC";
	if (0 != limit)
		queryString += " LIMIT :limit";

	QVector<Message> messages;
	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString("yyyyMMdd"));
	if (limit != 0)
		query.bindValue(":limit", limit);
	executeQuery(query);
	messages = messagesFromQuery(query);

	return messages;
}

QVector<Message> HistorySqlStorage::messagesSince(const Chat &chat, const QDate &date)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QVector<Message> messages;
	if (date.isNull())
		return messages;

	QSqlQuery query(Database);
	QString queryString = "SELECT chat.uuid, con.uuid, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) "
			"LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) "
			"LEFT JOIN kadu_contacts con ON (kadu_messages.contact_id=con.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE " + chatWhere(chat) +
			" AND date >= :date ORDER BY date_id ASC, kadu_messages.rowid ASC";
	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);

	messages = messagesFromQuery(query);

	return messages;
}

QVector<Message> HistorySqlStorage::messagesBackTo(const Chat &chat, const QDateTime &datetime, int limit)
{
	DatabaseMutex.lock();

	QVector<Message> result;

	QSqlQuery query(Database);
	// we want last *limit* messages, so we have to invert sorting here
	// it is reverted back manually below
	QString queryString = "SELECT chat.uuid, con.uuid, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) "
			"LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) "
			"LEFT JOIN kadu_contacts con ON (kadu_messages.contact_id=con.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE " + chatWhere(chat) +
			" AND date >= :date ORDER BY date_id DESC, kadu_messages.rowid DESC LIMIT :limit";
	query.prepare(queryString);

	query.bindValue(":chat", chat.uuid().toString());
	query.bindValue(":date", datetime.toString("yyyyMMdd"));
	query.bindValue(":limit", limit);

	executeQuery(query);

	result = messagesFromQuery(query);

	DatabaseMutex.unlock();

	// se comment above
	QVector<Message> inverted;
	inverted.reserve(result.size());

	for (int i = result.size() - 1; i >= 0; --i)
		inverted.append(result.at(i));
	return inverted;
}

QList<QString> HistorySqlStorage::smsRecipientsList(const HistorySearchParameters &search)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT receipient FROM kadu_sms WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND substr(send_time,0,11)  >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND substr(send_time,0,11)  <= :toDate";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":content", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QList<QString> recipients;

	executeQuery(query);

	while (query.next())
		recipients.append(query.value(0).toString());

	return recipients;
}

QVector<DatesModelItem> HistorySqlStorage::datesForSmsRecipient(const QString &recipient, const HistorySearchParameters &search)
{
	kdebugf();

	if (recipient.isEmpty())
		return QVector<DatesModelItem>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(send_time,0,11)";
	queryString += " FROM (SELECT send_time FROM kadu_sms WHERE receipient = :receipient";

	if (!search.query().isEmpty())
		queryString += " AND content LIKE :content";
	if (search.fromDate().isValid())
		queryString += " AND substr(send_time,0,11) >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND substr(send_time,0,11) <= :toDate";

	queryString += " order by send_time DESC, rowid DESC)";
	queryString += " group by substr(send_time,0,11) order by send_time ASC;";

	query.prepare(queryString);

	query.bindValue(":receipient", recipient);
	if (!search.query().isEmpty())
		query.bindValue(":content", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QVector<DatesModelItem> dates;
	executeQuery(query);

	while (query.next())
	{
		QDate date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		dates.append(DatesModelItem(date, query.value(0).toInt(), QString()));
	}

	return dates;
}

QVector<Message> HistorySqlStorage::sms(const QString &recipient, const QDate &date, int limit)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT content, send_time FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND substr(send_time,0,11) = :date";
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

	QVector<Message> result = smsFromQuery(query);

	return result;
}

QVector<Buddy> HistorySqlStorage::statusBuddiesList(const HistorySearchParameters &search)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT DISTINCT contact FROM kadu_statuses WHERE 1";

	if (!search.query().isEmpty())
		queryString += " AND description LIKE :description";
	if (search.fromDate().isValid())
		queryString += " AND substr(set_time,0,11) >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND substr(set_time,0,11) <= :toDate";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":description", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QVector<Buddy> buddies;
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

	return buddies;
}

QVector<DatesModelItem> HistorySqlStorage::datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search)
{
	kdebugf();

	if (!buddy)
		return QVector<DatesModelItem>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(set_time,0,11) FROM";
	queryString += " (select set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);

	if (!search.query().isEmpty())
		queryString += " AND description LIKE :description";
	if (search.fromDate().isValid())
		queryString += " AND substr(set_time,0,11) >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND substr(set_time,0,11) <= :toDate";

	queryString += "order by set_time DESC, rowid DESC)";
	queryString += "group by substr(set_time,0,11) order by set_time ASC";

	query.prepare(queryString);

	if (!search.query().isEmpty())
		query.bindValue(":description", QString('%' + search.query() + '%'));
	if (search.fromDate().isValid())
		query.bindValue(":fromDate", search.fromDate());
	if (search.toDate().isValid())
		query.bindValue(":toDate", search.toDate());

	QVector<DatesModelItem> dates;

	executeQuery(query);

	QDate date;
	while (query.next())
	{
		date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		dates.append(DatesModelItem(date, query.value(0).toInt(), QString()));
	}

	return dates;
}

QList<TimedStatus> HistorySqlStorage::statuses(const Buddy &buddy, const QDate &date, int limit)
{
	kdebugf();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT contact, status, description, set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy);
	if (!date.isNull())
		queryString += " AND substr(set_time,0,11) = :date";
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

	return statuses;
}

void HistorySqlStorage::executeQuery(QSqlQuery &query)
{
	kdebugf();

	query.setForwardOnly(true);

	QDateTime before = QDateTime::currentDateTime();
	query.exec();
	QDateTime after = QDateTime::currentDateTime();
	kdebugm(KDEBUG_INFO, "db query: %s\n", qPrintable(query.executedQuery()));
	printf("[%s]\n[%d.%d]-[%d.%d]/%d.%d\n", qPrintable(query.executedQuery()),
			before.toTime_t(), before.time().msec(),
			after.toTime_t(), after.time().msec(),
			after.toTime_t() - before.toTime_t(),
			after.time().msec() - before.time().msec());
}

QVector<Message> HistorySqlStorage::messagesFromQuery(QSqlQuery &query)
{
	QVector<Message> messages;
	while (query.next())
	{
		bool outgoing = query.value(5).toBool();

		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat.isNull())
			continue;

		MessageType type = outgoing ? MessageTypeSent : MessageTypeReceived;

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
		message.setStatus(outgoing ? MessageStatusDelivered : MessageStatusReceived);

		messages.append(message);
	}

	return messages;
}

QList<TimedStatus> HistorySqlStorage::statusesFromQuery(QSqlQuery &query)
{
	QList<TimedStatus> statuses;

	while (query.next())
	{
		// ignore non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(0).toString());
		if (sender.isNull())
			continue;

		Status status;
		status.setType(StatusTypeManager::instance()->fromName(query.value(1).toString()));
		status.setDescription(query.value(2).toString());

		TimedStatus timedStatus(status, query.value(3).toDateTime());

		statuses.append(timedStatus);
	}

	return statuses;
}

QVector<Message> HistorySqlStorage::smsFromQuery(QSqlQuery &query)
{
	QVector<Message> messages;

	while (query.next())
	{
		Message message = Message::create();
		message.setStatus(MessageStatusSent);
		message.setType(MessageTypeSystem);
		message.setReceiveDate(query.value(1).toDateTime());
		message.setSendDate(query.value(1).toDateTime());
		message.setContent(query.value(0).toString());

		messages.append(message);
	}

	return messages;
}
