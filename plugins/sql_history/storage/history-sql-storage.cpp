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
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
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
#include "gui/windows/progress-window2.h"
#include "message/formatted-message.h"
#include "message/message.h"
#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "status/status-type-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/talkable.h"
#include "debug.h"

#include "plugins/history/history.h"
#include "plugins/history/model/dates-model-item.h"
#include "plugins/history/search/history-search-parameters.h"
#include "plugins/history/timed-status.h"

#include "storage/sql-initializer.h"

#include "history-sql-storage.h"

#define DATE_TITLE_LENGTH 120

HistorySqlStorage::HistorySqlStorage(QObject *parent) :
		HistoryStorage(parent), ImportProgressWindow(0), DatabaseMutex(QMutex::NonRecursive)
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"),
				tr("It seems your Qt library does not provide support for selected database. "
				   "Please install Qt with %1 plugin.").arg("QSQLITE"));
		return;
	}

	qRegisterMetaType<QSqlError>("QSqlError");

	InitializerThread = new QThread();

	// this object cannot have parent as it will be moved to a new thread
	SqlInitializer *initializer = new SqlInitializer();
	initializer->moveToThread(InitializerThread);

	connect(InitializerThread, SIGNAL(started()), initializer, SLOT(initialize()));
	connect(initializer, SIGNAL(initialized()), InitializerThread, SLOT(quit()));
	connect(initializer, SIGNAL(databaseReady(bool)), this, SLOT(databaseReady(bool)));
	connect(initializer, SIGNAL(importStarted()), this, SLOT(importStarted()));
	connect(initializer, SIGNAL(importFinished()), this, SLOT(importFinished()));
	connect(initializer, SIGNAL(databaseOpenFailed(QSqlError)), this, SLOT(databaseOpenFailed(QSqlError)));

	InitializerThread->start();

	History::instance()->registerStorage(this);
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

	if (InitializerThread && InitializerThread->isRunning())
	{
		InitializerThread->terminate();
		InitializerThread->wait(2000);
	}

	delete InitializerThread;
	InitializerThread = 0;

	if (Database.isOpen())
		Database.commit();
}

void HistorySqlStorage::databaseReady(bool ok)
{
	if (ok)
		Database = QSqlDatabase::database("kadu-history", true);

	if (!Database.isOpen())
	{
		databaseOpenFailed(Database.lastError());
		History::instance()->unregisterStorage(this);
		return;
	}

	Database.transaction();
	initQueries();
}

void HistorySqlStorage::importStarted()
{
	ImportProgressWindow = new ProgressWindow2(
	            tr("Optimizing history database. This can take several minutes.\n"
	               "Please do not close Kadu until optimalization is complete.")
	);
	ImportProgressWindow->show();
}

void HistorySqlStorage::importFinished()
{
	if (ImportProgressWindow)
	{
		ImportProgressWindow->setText(tr("Optimalization complete. You can now close this window."));
		ImportProgressWindow->enableClosing();
	}
}

void HistorySqlStorage::databaseOpenFailed (const QSqlError &error)
{
	if (ImportProgressWindow)
	{
		ImportProgressWindow->setText(tr("Optimalization failed. Error message:\n%1").arg(error.text()));
		ImportProgressWindow->enableClosing();
	}
	else
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), error.text());
}

bool HistorySqlStorage::isDatabaseReady(bool wait)
{
	if (InitializerThread && InitializerThread->isRunning())
	{
		if (wait)
			InitializerThread->wait();
		else
			return false;
	}

	return Database.isOpen();
}

void HistorySqlStorage::initQueries()
{
	AppendMessageQuery = QSqlQuery(Database);
	AppendMessageQuery.prepare("INSERT INTO kadu_messages (chat_id, contact_id, send_time, receive_time, date_id, is_outgoing, content_id) VALUES "
			"(:chat_id, :contact_id, :send_time, :receive_time, :date_id, :is_outgoing, :content_id)");

	AppendStatusQuery = QSqlQuery(Database);
	AppendStatusQuery.prepare("INSERT INTO kadu_statuses (contact, status, set_time, description) VALUES "
			"(:contact, :status, :set_time, :description)");

	AppendSmsQuery = QSqlQuery(Database);
	AppendSmsQuery.prepare("INSERT INTO kadu_sms (receipient, send_time, content) VALUES "
			"(:receipient, :send_time, :content)");
}

QString HistorySqlStorage::chatWhere(const Chat &chat, const QString &chatPrefix)
{
	if (!chat)
		return QLatin1String("false");

	ChatDetails *details = chat.details();
	if (!details)
		return QLatin1String("false");

	ChatDetailsAggregate *aggregate = qobject_cast<ChatDetailsAggregate *>(details);
	if (!aggregate)
		return QString("%1uuid = '%2'").arg(chatPrefix).arg(chat.uuid().toString());

	QStringList uuids;
	foreach (const Chat &aggregatedChat, aggregate->chats())
		uuids.append(QString("'%1'").arg(aggregatedChat.uuid().toString()));

	return QString("%1uuid IN (%2)").arg(chatPrefix).arg(uuids.join(QLatin1String(", ")));
}

QString HistorySqlStorage::buddyContactsWhere(const Buddy &buddy, const QString &fieldName)
{
	if (!buddy || buddy.contacts().isEmpty())
		return  QLatin1String("false");

	QStringList uuids;
	foreach (const Contact &contact, buddy.contacts())
		uuids.append(QString("'%1'").arg(contact.uuid().toString()));

	return QString("(%1) IN (%2)").arg(fieldName).arg(uuids.join(QLatin1String(", ")));
}

void HistorySqlStorage::sync()
{
	if (!isDatabaseReady(false))
		return; // nothing to sync yet

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
	if (ChatMap.contains(chat))
		return ChatMap.value(chat);

	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_chats WHERE uuid=:uuid";

	query.prepare(queryString);
	query.bindValue(":uuid", chat.uuid().toString());

	int chatId = -1;

	executeQuery(query);

	if (query.next())
	{
		chatId = query.value(0).toInt();
		Q_ASSERT(!query.next());
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

	ChatMap.insert(chat, chatId);

	return chatId;
}

int HistorySqlStorage::findOrCreateContact(const Contact &contact)
{
	if (ContactMap.contains(contact))
		return ContactMap.value(contact);

	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_contacts WHERE uuid=:uuid";

	query.prepare(queryString);
	query.bindValue(":uuid", contact.uuid().toString());

	int contactId = -1;

	executeQuery(query);

	if (query.next())
	{
		contactId = query.value(0).toInt();
		Q_ASSERT(!query.next());
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

	ContactMap.insert(contact, contactId);

	return contactId;
}

int HistorySqlStorage::findOrCreateDate(const QDate &date)
{
	QString stringDate = date.toString("yyyyMMdd");
	if (DateMap.contains(stringDate))
		return DateMap.value(stringDate);

	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_dates WHERE date=:date";

	query.prepare(queryString);
	query.bindValue(":date", stringDate);

	int dateId = -1;

	executeQuery(query);

	if (query.next())
	{
		dateId = query.value(0).toInt();
		Q_ASSERT(!query.next());
	}
	else
	{
		QSqlQuery query(Database);
		QString queryString = "INSERT INTO kadu_dates (date) VALUES (:date)";

		query.prepare(queryString);
		query.bindValue(":date", stringDate);

		executeQuery(query);
		dateId = query.lastInsertId().toInt();
	}

	DateMap.insert(stringDate, dateId);

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

	if (!isDatabaseReady(true))
		return;

	QMutexLocker locker(&DatabaseMutex);

	int outgoing = (message.type() == MessageTypeSent)
			? 1
			: 0;

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

	if (!isDatabaseReady(true))
		return;

	QMutexLocker locker(&DatabaseMutex);

	StatusTypeData statusTypeData = StatusTypeManager::instance()->statusTypeData(status.type());

	AppendStatusQuery.bindValue(":contact", contact.uuid().toString());
	AppendStatusQuery.bindValue(":status", statusTypeData.name());
	AppendStatusQuery.bindValue(":set_time", time);
	AppendStatusQuery.bindValue(":description", status.description());

	executeQuery(AppendStatusQuery);

	AppendStatusQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::appendSms(const QString &recipient, const QString &content, const QDateTime &time)
{
	kdebugf();

	if (!isDatabaseReady(true))
		return;

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
	if (!isDatabaseReady(true))
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages WHERE chat_id IN (SELECT id FROM kadu_chats chat WHERE " + chatWhere(chat) + ")";
	if (!date.isNull())
		queryString += " AND date_id IN (SELECT id FROM kadu_dates WHERE date = :date)";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);

	QString removeChatsQueryString = "DELETE FROM kadu_chats WHERE " + chatWhere(chat, "") +
	        " AND 0 = (SELECT count(*) FROM kadu_messages WHERE chat_id = kadu_chats.id)";

	QSqlQuery removeChatsQuery(Database);

	removeChatsQuery.prepare(removeChatsQueryString);

	executeQuery(removeChatsQuery);
}

void HistorySqlStorage::clearStatusHistory(const Buddy &buddy, const QDate &date)
{
	if (!isDatabaseReady(true))
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy, "contact");
	if (!date.isNull())
		queryString += " AND substr(set_time,0,11) = :date";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::clearSmsHistory(const QString &recipient, const QDate &date)
{
	if (!isDatabaseReady(true))
		return;

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
	if (!isDatabaseReady(true))
		return;

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

	QString queryString = "DELETE FROM kadu_statuses WHERE " + buddyContactsWhere(buddy, "contact");
	query.prepare(queryString);
	executeQuery(query);
}

QVector<Chat> HistorySqlStorage::chats(const HistorySearchParameters &search)
{
	kdebugf();

	if (!isDatabaseReady(false))
		return QVector<Chat>();

	QMutexLocker locker(&DatabaseMutex);

	QString joins = !search.query().isEmpty() || search.fromDate().isValid() || search.toDate().isValid()
		? "LEFT JOIN kadu_messages km ON (kadu_chats.id=km.chat_id) LEFT JOIN kadu_dates kd ON (kd.id=km.date_id) LEFT JOIN kadu_message_contents kmc ON (kmc.id=km.content_id) "
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

	if (!isDatabaseReady(false))
		return QVector<DatesModelItem>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), date, content FROM";
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
	queryString += " ORDER BY date ASC, rowid ASC";

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

		if (title.length() > DATE_TITLE_LENGTH)
		{
			title.truncate(DATE_TITLE_LENGTH);
			title += " ...";
		}

		dates.append(DatesModelItem(date, title, count));
	}

	return dates;
}

QVector<Message> HistorySqlStorage::messages(const Chat &chat, const QDate &date, int limit)
{
	kdebugf();

	if (!isDatabaseReady(false))
		return QVector<Message>();

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

QVector<Message> HistorySqlStorage::getMessagesSince(const Chat &chat, const QDate &date)
{
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

	query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);

	messages = messagesFromQuery(query);

	return messages;
}

QVector<Message> HistorySqlStorage::syncMessagesSince(const Chat &chat, const QDate &date)
{
	if (!isDatabaseReady(true))
		return QVector<Message>();

	return getMessagesSince(chat, date);
}

QFuture<QVector<Message> > HistorySqlStorage::asyncMessagesSince(const Chat &chat, const QDate &date)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncMessagesSince, chat, date);
}

QVector<Message> HistorySqlStorage::getMessagesBackTo(const Chat &chat, const QDateTime &datetime, int limit)
{
	DatabaseMutex.lock();

	QVector<Message> result;

	QSqlQuery query(Database);
	// we want last *limit* messages, so we have to invert sorting here
	// it is reverted back manually below
	QString queryString = "SELECT chat.uuid, con.uuid, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) "
			"LEFT JOIN kadu_contacts con ON (kadu_messages.contact_id=con.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE " + chatWhere(chat) +
			" AND receive_time >= :datetime ORDER BY date_id DESC, kadu_messages.rowid DESC LIMIT :limit";
	query.prepare(queryString);

	query.bindValue(":datetime", datetime.toString(Qt::ISODate));
	query.bindValue(":limit", limit);

	executeQuery(query);

	result = messagesFromQuery(query);

	DatabaseMutex.unlock();

	// see comment above
	QVector<Message> inverted;
	inverted.reserve(result.size());

	for (int i = result.size() - 1; i >= 0; --i)
		inverted.append(result.at(i));
	return inverted;
}

QVector<Message> HistorySqlStorage::syncGetMessagesBackTo(const Chat &chat, const QDateTime &datetime, int limit)
{
	if (!isDatabaseReady(true))
		return QVector<Message>();

	return getMessagesBackTo(chat, datetime, limit);
}

QFuture<QVector<Message> > HistorySqlStorage::asyncMessagesBackTo(const Chat &chat, const QDateTime &datetime, int limit)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncGetMessagesBackTo, chat, datetime, limit);
}

QList<QString> HistorySqlStorage::smsRecipientsList(const HistorySearchParameters &search)
{
	kdebugf();

	if (!isDatabaseReady(false))
		return QList<QString>();

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

	if (!isDatabaseReady(false))
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

		dates.append(DatesModelItem(date, QString(), query.value(0).toInt()));
	}

	return dates;
}

QVector<Message> HistorySqlStorage::sms(const QString &recipient, const QDate &date, int limit)
{
	kdebugf();

	if (!isDatabaseReady(false))
		return QVector<Message>();

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

	if (!isDatabaseReady(false))
		return QVector<Buddy>();

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

	executeQuery(query);
	while (query.next())
	{
		Contact contact = ContactManager::instance()->byUuid(query.value(0).toString());
		if (!contact)
			continue;

		Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);
		Q_ASSERT(buddy);
		if (!buddies.contains(buddy))
			buddies.append(buddy);
	}

	return buddies;
}

QVector<DatesModelItem> HistorySqlStorage::datesForStatusBuddy(const Buddy &buddy, const HistorySearchParameters &search)
{
	kdebugf();

	if (!buddy)
		return QVector<DatesModelItem>();

	if (!isDatabaseReady(false))
		return QVector<DatesModelItem>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(set_time,0,11) FROM";
	queryString += " (SELECT set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy, "contact");

	if (!search.query().isEmpty())
		queryString += " AND description LIKE :description";
	if (search.fromDate().isValid())
		queryString += " AND substr(set_time,0,11) >= :fromDate";
	if (search.toDate().isValid())
		queryString += " AND substr(set_time,0,11) <= :toDate";

	queryString += " ORDER BY set_time DESC, rowid DESC)";
	queryString += " GROUP BY substr(set_time,0,11) ORDER BY set_time ASC";

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

		dates.append(DatesModelItem(date, QString(), query.value(0).toInt()));
	}

	return dates;
}

QList<TimedStatus> HistorySqlStorage::statuses(const Buddy &buddy, const QDate &date, int limit)
{
	kdebugf();

	if (!isDatabaseReady(false))
		return QList<TimedStatus>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT contact, status, description, set_time FROM kadu_statuses WHERE " + buddyContactsWhere(buddy, "contact");
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

/*
	printf("[%s]\n[%d.%d]-[%d.%d]/%d.%d\n", qPrintable(query.executedQuery()),
			before.toTime_t(), before.time().msec(),
			after.toTime_t(), after.time().msec(),
			after.toTime_t() - before.toTime_t(),
			after.time().msec() - before.time().msec());
*/
}

QString HistorySqlStorage::stripAllScriptTags(const QString &string)
{
	QString beforeReplace = string;
	QString afterReplace = beforeReplace;

	afterReplace.replace("<script", "", Qt::CaseInsensitive);
	while (beforeReplace != afterReplace)
	{
		beforeReplace = afterReplace;
		afterReplace.replace("<script", "", Qt::CaseInsensitive);
	}

	return afterReplace;
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
		message.setContent(stripAllScriptTags(query.value(2).toString()));
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
		status.setDescription(Qt::escape(query.value(2).toString()));

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
		message.setContent(Qt::escape(query.value(0).toString()));

		messages.append(message);
	}

	return messages;
}
