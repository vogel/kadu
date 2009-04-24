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
	@short Klasa obs³ugi zapisu historii rozmów w bazach danych SQLite, MySQL i PostgreSQL.
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QSqlDatabase Database; /*!< Obiekt reprezentuj±cy bie¿±c± bazê danych. */
	QSqlTableModel *MessagesModel;
	QString tableNamePrefix;

	/**
		\fn void appendHistory(ChatWidget *chat);
		Funkcja dodaje zapis historii rozmów do nowo tworzonego okna rozmowy.
		\param chat okno rozmowy
	**/
	void appendHistory(ChatWidget *chat);

	/**
		\fn void removeHistory(const ContactList& uids);
		Funkcja usuwa ca³± historiê dla podanej listy u¿ytkowników.
		\param uids lista u¿ytkowników
	**/
	//void removeHistory(const ContactList& uids);
		
	void executeQuery(const QString &query);
	/**
		\fn QString prepareText(const QString &text);
		Funkcja zabezpiecza w tek¶cie znaki mog±ce potencjalnie rozwaliæ zapytanie do bazy.
		\param text tekst do przeczyszczenia
		\return wyczyszczony tekst
	**/
	QString prepareText(const QString &text);	
 
private slots:
	virtual void messageReceived(Chat *chat, Contact contact, const QString &message);
	virtual void messageSent(Chat *chat, const QString &message);
	void initializeDatabase();

public:
	HistorySqlStorage();
	~HistorySqlStorage();

	QList<QDate> getAllDates();
	QList<QDate> historyDates(const ContactList& uids);
	QList<QDate> historyStatusDates(const ContactList& uids);
	QList<QDate> historySmsDates(const ContactList& uids);

	QList<ChatMessage*> historyMessages(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getStatusEntries(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getSmsEntries(const ContactList& uids, QDate date = QDate());

	int getEntriesCount(const QList<ContactList> &uids, HistoryEntryType type);
	/**
		\fn void appendMessageEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje wiadomo¶ci w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendMessageEntry(Chat *chat, Contact contact, const QString &message);
	/**
		\fn void appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje smsy w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	/**
		\fn void appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &adds);
		Zapisuje statusy w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &adds);

	/**
		\fn void removeHistory(const ContactList& uids, const QDate &date = 0);
		Funkcja usuwa ca³± historiê dla podanej listy u¿ytkowników lub tylko dla podanej daty.
		\param uids lista u¿ytkowników
	**/
	void removeHistory(const ContactList& uids, const QDate &date = QDate(), HistoryEntryType type = EntryTypeMessage);
	
public slots:
	void appendSms(const QString &mobile, const QString &msg);
	void appendStatus(Contact elem, QString protocolName);
	HistorySearchResult searchHistory(ContactList users, HistorySearchParameters params);

};

#endif
