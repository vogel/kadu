#ifndef HISTORY_SQL_STORAGE_H
#define HISTORY_SQL_STORAGE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlTableModel>

#include "../../history/history.h"
#include "../../history/storage/history-storage.h"

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QSqlDatabase Database;
	QSqlTableModel *MessagesModel;
	int MessagesAdded;

	QSqlQuery ClearChatHistoryQuery;
	
	void initDatabase();
	void initQueries();
	void initTables();
	void initKaduMessagesTable();

	void submitRemaining();
	void submitRemainingKaduMessages();

	void appendHistory(ChatWidget *chat);
	void executeQuery(QSqlQuery query);
	QString prepareText(const QString &text);
 
private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);

public:
	HistorySqlStorage();
	~HistorySqlStorage();

	virtual QList<Chat *> chatsList();
	virtual QList<QDate> datesForChat(Chat *chat);
	virtual QList<ChatMessage *> messages(Chat *chat, QDate date = QDate(), int limit = 0);
	virtual int messagesCount(Chat *chat, QDate date = QDate());

	QList<QDate> getAllDates();
	QList<QDate> historyDates(const ContactList& uids);
	QList<QDate> historyStatusDates(const ContactList& uids);
	QList<QDate> historySmsDates(const ContactList& uids);

	QList<ChatMessage*> historyMessages(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getStatusEntries(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getSmsEntries(const ContactList& uids, QDate date = QDate());

	int getEntriesCount(const QList<ContactList> &uids, HistoryEntryType type);

	void appendMessage(const Message &message);
	void appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	void appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &adds);

	void clearChatHistory(Chat *chat);

	void removeHistory(const ContactList& uids, const QDate &date = QDate(), HistoryEntryType type = EntryTypeMessage);
	
public slots:
	void appendSms(const QString &mobile, const QString &msg);
	void appendStatus(Contact elem, QString protocolName);
	HistorySearchResult searchHistory(ContactList users, HistorySearchParameters params);

};

#endif
