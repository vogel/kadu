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
#include "talkable/talkable.h"
#include "debug.h"

#include "plugins/history/history.h"
#include "plugins/history/search/history-search-parameters.h"
#include "plugins/history/history-query.h"
#include "plugins/history/history-query-result.h"

#include "storage/sql-initializer.h"
#include "storage/sql-messages-chat-storage.h"
#include "storage/sql-messages-sms-storage.h"
#include "storage/sql-messages-status-storage.h"

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

	ChatStorage = new SqlMessagesChatStorage(this);
	StatusStorage = new SqlMessagesStatusStorage(this);
	SmsStorage = new SqlMessagesSmsStorage(this);

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

bool HistorySqlStorage::isDatabaseReady()
{
	if (InitializerThread && InitializerThread->isRunning())
		return false;

	return Database.isOpen();
}

bool HistorySqlStorage::waitForDatabase()
{
	if (InitializerThread && InitializerThread->isRunning())
		InitializerThread->wait();

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
		return QLatin1String("0");

	ChatDetailsAggregate *aggregate = qobject_cast<ChatDetailsAggregate *>(chat.details());
	if (!aggregate)
		return QString("%1uuid = '%2'").arg(chatPrefix).arg(chat.uuid().toString());

	QStringList uuids;
	foreach (const Chat &aggregatedChat, aggregate->chats())
		uuids.append(QString("'%1'").arg(aggregatedChat.uuid().toString()));

	return QString("%1uuid IN (%2)").arg(chatPrefix).arg(uuids.join(QLatin1String(", ")));
}

QString HistorySqlStorage::talkableContactsWhere(const Talkable &talkable, const QString &fieldName)
{
	if (talkable.isValidBuddy())
		return buddyContactsWhere(talkable.toBuddy(), fieldName);
	else if (talkable.isValidContact())
		return QString("(%1) = '%2'").arg(fieldName).arg(talkable.toContact().uuid().toString());

	return QLatin1String("1");
}

QString HistorySqlStorage::buddyContactsWhere(const Buddy &buddy, const QString &fieldName)
{
	if (!buddy || buddy.contacts().isEmpty())
		return QLatin1String("0");

	QStringList uuids;
	foreach (const Contact &contact, buddy.contacts())
		uuids.append(QString("'%1'").arg(contact.uuid().toString()));

	return QString("(%1) IN (%2)").arg(fieldName).arg(uuids.join(QLatin1String(", ")));
}

void HistorySqlStorage::sync()
{
	if (!isDatabaseReady())
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

Chat HistorySqlStorage::findChat(int id)
{
	if (ChatMap.values().contains(id))
		return ChatMap.key(id);

	QSqlQuery query(Database);
	QString queryString = "SELECT uuid FROM kadu_chats WHERE id=:id";

	query.prepare(queryString);
	query.bindValue(":id", id);

	Chat chat;

	executeQuery(query);

	if (query.next())
	{
		chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (!chat)
		{
			chat = Chat::create();
			chat.setUuid(query.value(0).toString());
			chat.setDisplay("?");
		}

		Q_ASSERT(!query.next());
	}

	ChatMap.insert(chat, id);

	return chat;
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

	if (!waitForDatabase())
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

	if (!waitForDatabase())
		return;

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

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	AppendSmsQuery.bindValue(":contact", recipient);
	AppendSmsQuery.bindValue(":send_time", time);
	AppendSmsQuery.bindValue(":content", content);

	executeQuery(AppendSmsQuery);

	AppendSmsQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(const Talkable &talkable, const QDate &date)
{
	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages WHERE chat_id IN (SELECT id FROM kadu_chats chat WHERE " + chatWhere(talkable.toChat()) + ")";
	if (!date.isNull())
		queryString += " AND date_id IN (SELECT id FROM kadu_dates WHERE date = :date)";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);

	QString removeChatsQueryString = "DELETE FROM kadu_chats WHERE " + chatWhere(talkable.toChat(), "") +
	        " AND 0 = (SELECT count(*) FROM kadu_messages WHERE chat_id = kadu_chats.id)";

	QSqlQuery removeChatsQuery(Database);

	removeChatsQuery.prepare(removeChatsQueryString);

	executeQuery(removeChatsQuery);
}

void HistorySqlStorage::clearStatusHistory(const Talkable &talkable, const QDate &date)
{
	if (!talkable.isValidBuddy() && !talkable.isValidContact())
		return;

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_statuses WHERE " + talkableContactsWhere(talkable, "contact");
	if (!date.isNull())
		queryString += " AND substr(set_time,0,11) = :date";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::clearSmsHistory(const Talkable &talkable, const QDate &date)
{
	if (!talkable.isValidBuddy() || talkable.toBuddy().mobile().isEmpty())
		return;

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND substr(send_time,0,11) = :date";

	query.prepare(queryString);

	query.bindValue(":receipient", talkable.toBuddy().mobile());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::deleteHistory(const Talkable &talkable)
{
	foreach (const Contact &contact, talkable.toBuddy().contacts())
	{
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact), false);
		clearChatHistory(chat, QDate());
	}

	clearStatusHistory(talkable.toBuddy(), QDate());
}

QVector<Talkable> HistorySqlStorage::syncChats()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	query.prepare("SELECT uuid FROM kadu_chats");

	executeQuery(query);

	QVector<Talkable> result;
	while (query.next())
	{
		Chat chat = ChatManager::instance()->byUuid(query.value(0).toString());
		if (chat)
			result.append(chat);
	}

	return result;
}

QFuture<QVector<Talkable> > HistorySqlStorage::chats()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncChats);
}

QVector<Talkable> HistorySqlStorage::syncStatusBuddies()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	query.prepare("SELECT DISTINCT contact FROM kadu_statuses");
	executeQuery(query);

	QVector<Talkable> result;
	while (query.next())
	{
		Contact contact = ContactManager::instance()->byUuid(query.value(0).toString());
		if (!contact)
			continue;

		Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreate);
		Q_ASSERT(buddy);

		if (!result.contains(buddy))
			result.append(buddy);
	}

	return result;
}

QFuture<QVector<Talkable> > HistorySqlStorage::statusBuddies()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatusBuddies);
}

QVector<Talkable> HistorySqlStorage::syncSmsRecipients()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	query.prepare("SELECT DISTINCT receipient FROM kadu_sms");
	executeQuery(query);

	QVector<Talkable> result;
	while (query.next())
	{
		Buddy buddy = Buddy::create();
		buddy.setDisplay(query.value(0).toString());
		buddy.setMobile(query.value(0).toString());
		result.append(buddy);
	}

	return result;
}

QFuture<QVector<Talkable> > HistorySqlStorage::smsRecipients()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmsRecipients);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncChatDates(const HistoryQuery &historyQuery)
{
	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	const Talkable &talkable = historyQuery.talkable();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), date, content, chat_id FROM";
	queryString += " (SELECT km.rowid, date, date_id, chat_id, content FROM kadu_messages km "
		"LEFT JOIN kadu_message_contents kmc ON (km.content_id=kmc.id) "
		"LEFT JOIN kadu_dates d ON (km.date_id=d.id) "
		"LEFT JOIN kadu_chats chat ON (km.chat_id=chat.id) WHERE 1";

	if (!talkable.isEmpty())
		queryString += QString(" AND %1").arg(chatWhere(talkable.toChat()));
	if (!historyQuery.string().isEmpty())
		queryString += " AND kmc.content LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND date >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND date <= :toDate";

	queryString += " ORDER BY date_id DESC, km.rowid DESC )";
	queryString += " GROUP BY date_id, chat_id";
	queryString += " ORDER BY date_id ASC, rowid ASC";

	query.prepare(queryString);

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;
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
		QString title = formatted.toPlain().replace('\n', ' ').replace('\r', ' ');

		if (title.length() > DATE_TITLE_LENGTH)
		{
			title.truncate(DATE_TITLE_LENGTH);
			title += " ...";
		}

		Chat chat = findChat(query.value(3).toInt());

		HistoryQueryResult result;
		result.setTalkable(Talkable(chat));
		result.setDate(date);
		result.setTitle(title);
		result.setCount(count);
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult > > HistorySqlStorage::chatDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncChatDates, historyQuery);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncStatusDates(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(set_time,0,11), contact, description FROM";
	queryString += " (SELECT set_time, contact, description FROM kadu_statuses WHERE " + talkableContactsWhere(talkable, "contact");

	if (!historyQuery.string().isEmpty())
		queryString += " AND kadu_statuses.description LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY set_time DESC, rowid DESC)";
	queryString += " GROUP BY substr(set_time,0,11), contact ORDER BY set_time ASC";

	query.prepare(queryString);

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;

	executeQuery(query);

	QDate date;
	while (query.next())
	{
		date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		HistoryQueryResult result;

		Contact contact = ContactManager::instance()->byUuid(query.value(2).toString());
		if (contact)
		{
			const Buddy &buddy = BuddyManager::instance()->byContact(contact, ActionCreate);
			result.setTalkable(Talkable(buddy));
		}
		else
		{
			const Buddy &buddy = Buddy::create();
			buddy.setDisplay("?");

			contact = Contact::create("");
			contact.setUuid(query.value(2).toString());
			contact.setOwnerBuddy(buddy);

			result.setTalkable(Talkable(contact));
		}

		result.setDate(date);
		result.setTitle(query.value(3).toString().replace('\n', ' ').replace('\r', ' '));
		result.setCount(query.value(0).toInt());
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult> > HistorySqlStorage::statusDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatusDates, historyQuery);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncSmsRecipientDates(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(send_time,0,11), receipient, content";
	queryString += " FROM (SELECT send_time, receipient, content FROM kadu_sms WHERE ";

	if (talkable.isValidBuddy() && !talkable.toBuddy().mobile().isEmpty())
		queryString += "receipient = :receipient";
	else
		queryString += "1";

	if (!historyQuery.string().isEmpty())
		queryString += " AND kadu_sms.content LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') <= :toDate";

	queryString += " order by send_time DESC, rowid DESC)";
	queryString += " group by substr(send_time,0,11), receipient order by send_time ASC;";

	query.prepare(queryString);

	if (talkable.isValidBuddy() && !talkable.toBuddy().mobile().isEmpty())
		query.bindValue(":receipient", talkable.toBuddy().mobile());

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;
	executeQuery(query);

	while (query.next())
	{
		QDate date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		HistoryQueryResult result;

		Buddy buddy = Buddy::create();
		buddy.setDisplay(query.value(2).toString());
		buddy.setMobile(query.value(2).toString());

		result.setTalkable(Talkable(buddy));
		result.setDate(date);
		result.setTitle(query.value(3).toString().replace('\n', ' ').replace('\r', ' '));
		result.setCount(query.value(0).toInt());
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult> > HistorySqlStorage::smsRecipientDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmsRecipientDates, historyQuery);
}

QVector<Message> HistorySqlStorage::syncMessages(const HistoryQuery &historyQuery)
{
	if (!waitForDatabase())
		return QVector<Message>();

	QMutexLocker locker(&DatabaseMutex);

	const Talkable &talkable = historyQuery.talkable();

	QSqlQuery query(Database);
	QString queryString = "SELECT chat.uuid, con.uuid, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_chats chat ON (kadu_messages.chat_id=chat.id) "
			"LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) "
			"LEFT JOIN kadu_contacts con ON (kadu_messages.contact_id=con.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE 1";

	if (!talkable.isEmpty())
		queryString += QString(" AND %1").arg(chatWhere(talkable.toChat()));
	if (historyQuery.fromDate().isValid())
		queryString += " AND date >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND date <= :toDate";
	if (historyQuery.fromDateTime().isValid())
		queryString += " AND receive_time >= :fromDateTime";
	if (historyQuery.toDateTime().isValid())
		queryString += " AND receive_time <= :toDateTime";

	// we want last *limit* messages, so we have to invert sorting here
	// it is reverted back manually below
	if (historyQuery.limit() > 0)
	{
		queryString += " ORDER BY kadu_messages.date_id DESC, kadu_messages.rowid DESC";
		queryString += " LIMIT :limit";
	}
	else
		queryString += " ORDER BY kadu_messages.date_id ASC, kadu_messages.rowid ASC";

	query.prepare(queryString);

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));
	if (historyQuery.fromDateTime().isValid())
		query.bindValue(":fromDateTime", historyQuery.fromDateTime().toString(Qt::ISODate));
	if (historyQuery.toDateTime().isValid())
		query.bindValue(":toDateTime", historyQuery.toDateTime().toString(Qt::ISODate));

	if (historyQuery.limit() > 0)
		query.bindValue(":limit", historyQuery.limit());

	QVector<Message> messages;

	executeQuery(query);
	messages = messagesFromQuery(query);

	if (historyQuery.limit() > 0)
	{
		// see comment above
		QVector<Message> inverted;
		inverted.reserve(messages.size());

		for (int i = messages.size() - 1; i >= 0; --i)
			inverted.append(messages.at(i));
		return inverted;
	}

	return messages;
}

QFuture<QVector<Message> > HistorySqlStorage::messages(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncMessages, historyQuery);
}

QVector<Message> HistorySqlStorage::syncStatuses(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<Message>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT contact, status, description, set_time FROM kadu_statuses WHERE " + talkableContactsWhere(talkable, "contact");

	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY set_time ASC";

	QVector<Message> statuses;
	query.prepare(queryString);

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	executeQuery(query);
	statuses = statusesFromQuery(talkable.toContact(), query);

	return statuses;
}

QFuture<QVector<Message> > HistorySqlStorage::statuses(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatuses, historyQuery);
}

QVector<Message> HistorySqlStorage::syncSmses(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<Message>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT content, send_time FROM kadu_sms WHERE 1";

	if (talkable.isValidBuddy() && !talkable.toBuddy().mobile().isEmpty())
		queryString += " AND receipient = :receipient";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY send_time ASC";

	query.prepare(queryString);

	if (talkable.isValidBuddy() && !talkable.toBuddy().mobile().isEmpty())
		query.bindValue(":receipient", talkable.toBuddy().mobile());

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	executeQuery(query);

	QVector<Message> result = smsFromQuery(query);

	return result;
}

QFuture<QVector<Message> > HistorySqlStorage::smses(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmses, historyQuery);
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

QVector<Message> HistorySqlStorage::messagesFromQuery(QSqlQuery &query)
{
	QVector<Message> messages;
	while (query.next())
	{
		bool outgoing = query.value(5).toBool();

		MessageType type = outgoing ? MessageTypeSent : MessageTypeReceived;

		// allow displaying messages for non-existing contacts
		Contact sender = ContactManager::instance()->byUuid(query.value(1).toString());
		if (!sender)
		{
			Contact sender = Contact::create("");
			Buddy senderBuddy = Buddy::create();
			senderBuddy.setDisplay("?");
			sender.setOwnerBuddy(senderBuddy);
		}

		Message message = Message::create();
		message.setMessageChat(ChatManager::instance()->byUuid(query.value(0).toString()));
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

QVector<Message> HistorySqlStorage::statusesFromQuery(const Contact &contact, QSqlQuery &query)
{
	QVector<Message> statuses;

	while (query.next())
	{
		StatusType type = StatusTypeManager::instance()->fromName(query.value(1).toString());
		const StatusTypeData &typeData = StatusTypeManager::instance()->statusTypeData(type);

		Message message = Message::create();

		const QString description = query.value(2).toString();
		if (description.isEmpty())
			message.setContent(typeData.name());
		else
			message.setContent(QString("%1 with description: %2")
					.arg(typeData.name())
					.arg(description));

		message.setStatus(MessageStatusReceived);
		message.setType(MessageTypeSystem);
		message.setMessageSender(contact);
		message.setReceiveDate(query.value(3).toDateTime());
		message.setSendDate(query.value(3).toDateTime());

		statuses.append(message);
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

HistoryMessagesStorage * HistorySqlStorage::chatStorage()
{
	return ChatStorage;
}

HistoryMessagesStorage * HistorySqlStorage::smsStorage()
{
	return SmsStorage;
}

HistoryMessagesStorage * HistorySqlStorage::statusStorage()
{
	return StatusStorage;
}
