/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "plugins/history/storage/history-storage.h"

#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <injeqt/injeqt.h>

class QSqlError;

class BuddyChatManager;
class BuddyManager;
class BuddyStorage;
class ChatManager;
class ChatStorage;
class ContactStorage;
class FormattedStringFactory;
class HistoryQuery;
class IconsManager;
class PluginInjectedFactory;
class MessageStorage;
class ProgressWindow;
class StatusTypeManager;
class TalkableConverter;

class SqlAccountsMapping;
class SqlChatsMapping;
class SqlContactsMapping;

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QPointer<BuddyChatManager> m_buddyChatManager;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<BuddyStorage> m_buddyStorage;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<ContactStorage> m_contactStorage;
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<MessageStorage> m_messageStorage;
	QPointer<StatusTypeManager> m_statusTypeManager;
	QPointer<TalkableConverter> m_talkableConverter;

	QThread *InitializerThread;
	ProgressWindow *ImportProgressWindow;

	QSqlDatabase Database;
	SqlAccountsMapping *AccountsMapping;
	SqlContactsMapping *ContactsMapping;
	SqlChatsMapping *ChatsMapping;

	QSqlQuery AppendMessageQuery;
	QSqlQuery AppendStatusQuery;
	QSqlQuery AppendSmsQuery;

	QMutex DatabaseMutex;

	QMap<QString, int> DateMap;

	HistoryMessagesStorage *m_historyChatStorage;
	HistoryMessagesStorage *StatusStorage;
	HistoryMessagesStorage *SmsStorage;

	void initQueries();

	int saveMessageContent(const Message &message);
	int findOrCreateDate(const QDate &date);

	void ensureProgressWindowReady();

	QString chatIdList(const Chat &chat);
	QString talkableContactsWhere(const Talkable &talkable);
	QString buddyContactsWhere(const Buddy &buddy);

	void executeQuery(QSqlQuery &query);
	SortedMessages messagesFromQuery(QSqlQuery &query);
	SortedMessages statusesFromQuery(const Contact &contact, QSqlQuery &query);
	SortedMessages smsFromQuery(QSqlQuery &query);

	bool isDatabaseReady();
	bool waitForDatabase();

	QVector<Talkable> syncChats();
	QVector<Talkable> syncStatusBuddies();
	QVector<Talkable> syncSmsRecipients();

	QVector<HistoryQueryResult> syncChatDates(const HistoryQuery &historyQuery);
	QVector<HistoryQueryResult> syncStatusDates(const HistoryQuery &historyQuery);
	QVector<HistoryQueryResult> syncSmsRecipientDates(const HistoryQuery &historyQuery);

	SortedMessages syncMessages(const HistoryQuery &historyQuery);
	SortedMessages syncStatuses(const HistoryQuery &historyQuery);
	SortedMessages syncSmses(const HistoryQuery &historyQuery);

private slots:
	INJEQT_SET void setBuddyChatManager(BuddyChatManager *buddyChatManager);
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setBuddyStorage(BuddyStorage *buddyStorage);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setContactStorage(ContactStorage *contactStorage);
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

	void initializerProgressMessage(const QString &iconName, const QString &message);
	void initializerProgressFinished(bool ok, const QString &iconName, const QString &message);

	void databaseReady(bool ok);

public:
	Q_INVOKABLE explicit HistorySqlStorage(QObject *parent = nullptr);
	virtual ~HistorySqlStorage();

	virtual QFuture<QVector<Talkable>> chats();
	virtual QFuture<QVector<Talkable>> statusBuddies();
	virtual QFuture<QVector<Talkable>> smsRecipients();

	virtual QFuture<QVector<HistoryQueryResult>> chatDates(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<HistoryQueryResult>> statusDates(const HistoryQuery &historyQuery);
	virtual QFuture<QVector<HistoryQueryResult>> smsRecipientDates(const HistoryQuery &historyQuery);

	virtual QFuture<SortedMessages> messages(const HistoryQuery &historyQuery);
	virtual QFuture<SortedMessages> statuses(const HistoryQuery &historyQuery);
	virtual QFuture<SortedMessages> smses(const HistoryQuery &historyQuery);

	virtual void appendMessage(const Message &message);
	virtual void appendStatus(const Contact &contact, const Status &status, const QDateTime &time);
	virtual void appendSms(const QString &recipient, const QString &content, const QDateTime &time);

	void sync();

	virtual void clearChatHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearSmsHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void clearStatusHistory(const Talkable &talkable, const QDate &date = QDate());
	virtual void deleteHistory(const Talkable &talkable);

	virtual HistoryMessagesStorage * chatStorage();
	virtual HistoryMessagesStorage * statusStorage();
	virtual HistoryMessagesStorage * smsStorage();

};
