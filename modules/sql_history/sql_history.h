#ifndef NEW_HISTORY_H
#define NEW_HISTORY_H

#include <QtCore/QObject>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtSql/QtSql>
#include <QtGui/QMenu>
#include <QtGui/qcheckbox.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>

#include "chat_edit_box.h"
#include "chat_message.h"
#include "chat_widget.h"
#include "config_file.h"
#include "html_document.h"
#include "protocol.h"
#include "toolbar.h"
#include "usergroup.h"
#include "main_configuration_window.h"
#include "configuration_aware_object.h"
#include "configuration_window_widgets.h"

enum HistoryEntryType
{
	EntryTypeMessage = 0x00000001,
	EntryTypeStatus = 0x00000010,
	EntryTypeSms = 0x00000020,
	EntryTypeAll = 0x0000003f
};


#include "history_dialog.h"



struct HistorySearchResult;
struct HistorySearchParameters;

class Account;
class HistoryDlg;

/**
	@class SqlHistory
	@author Juzef, Adrian
	@short Klasa obs³ugi zapisu historii rozmów w bazach danych SQLite, MySQL i PostgreSQL.
**/

class SqlHistory : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	QLabel *dontCiteOldMessagesLbl;
	QListWidget *allStatusUsers;
	QListWidget *selectedStatusUsers;
	QListWidget *allChatsUsers;
	QListWidget *selectedChatsUsers; 
	QSpinBox *portSpinBox;	
	QComboBox *driverComboBox;
	ActionDescription *showHistoryActionDescription;  /*!< Akcja otwieraj±ca okno historii. */
	ActionDescription *clearHistoryActionDescription;
	QLineEdit* hostLineEdit;
	QLineEdit* userLineEdit; 
	QLineEdit* nameLineEdit;  
	QLineEdit* passLineEdit;
	QLineEdit* prefixLineEdit;
	HistoryDlg* historyDialog;

	QSqlDatabase debe; /*!< Obiekt reprezentuj±cy bie¿±c± bazê danych. */
	QString tableNamePrefix;
	/**
		\fn QString findNewUidGroupId();
		Funkcja zwraca ID dla nowo tworzonego pola grupy u¿ytkowników
	**/
	QString findNewUidGroupId();
	void updateQuoteTimeLabel(int);
	void createDefaultConfiguration(); 
	/**
		\fn void appendHistory(ChatWidget *chat);
		Funkcja dodaje zapis historii rozmów do nowo tworzonego okna rozmowy.
		\param chat okno rozmowy
	**/
	void appendHistory(ChatWidget *chat);

	/**
		\fn void removeHistory(const UserListElements& uids);
		Funkcja usuwa ca³± historiê dla podanej listy u¿ytkowników.
		\param uids lista u¿ytkowników
	**/
	//void removeHistory(const UserListElements& uids);
	QList<UserListElements> getUidGroups(QString queryString);
		
	void executeQuery(const QString &query);
	/**
		\fn QString prepareText(const QString &text);
		Funkcja zabezpiecza w tek¶cie znaki mog±ce potencjalnie rozwaliæ zapytanie do bazy.
		\param text tekst do przeczyszczenia
		\return wyczyszczony tekst
	**/
	QString prepareText(const QString &text);	
	/**
		\fn QString addUidGroup(UserListElements users);
		Funkcja tworzy rekord nowej grupy u¿ytkowników.
		\param users lista u¿ytkowników
		\return string z ID pola bazy danych nowo utworzonej grupy u¿ytkowników
	**/
	QString addUidGroup(UserListElements users);	
	ConfigurationWindow *historyAdvanced;

virtual void configurationUpdated();

private slots:
	void moveToSelectedStatusList();
	void moveToAllStatusList();
	void moveToSelectedChatsList();
	void moveToAllChatsList();
	void driverComboBoxValueChanged(int index);
		
	void historyActionActivated(QAction* sender, bool toggled);
	void clearHistoryActionActivated(QAction *sender, bool toggled);
	void initializeDatabase();
	void configurationWindowApplied();
	void viewHistory(QAction *sender, bool toggled);
	void userboxMenuPopup();
	void statusChanged(UserListElement elem, QString protocolName,
		const UserStatus &oldStatus, bool massively, bool last);
	void chatCreated(ChatWidget *chat);
	void chatDestroying(ChatWidget *chat);
	void chatKeyPressed(QKeyEvent *e, ChatWidget *widget, bool &handled);
	void removingUsers(UserListElements users);
	void portSpinBoxValueChanged(int value);
	void showHistoryAdvanced();
	void historyAdvancedDestroyed();

	// new API
	void accountRegistered(Account *);
	void accountUnregistered(Account *);

public:
	SqlHistory(bool firstLoad);
	~SqlHistory();

	QList<UserListElements> getAllUidGroups();
	QList<UserListElements> getChatUidGroups();
	QList<UserListElements> getStatusUidGroups();
	QList<UserListElements> getSmsUidGroups();

	QList<QDate> getAllDates();
	QList<QDate> historyDates(const UserListElements& uids);
	QList<QDate> historyStatusDates(const UserListElements& uids);
	QList<QDate> historySmsDates(const UserListElements& uids);

	QList<ChatMessage*> historyMessages(const UserListElements& uids, QDate date = QDate());
	QList<ChatMessage*> getStatusEntries(const UserListElements& uids, QDate date = QDate());
	QList<ChatMessage*> getSmsEntries(const UserListElements& uids, QDate date = QDate());

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	QString findUidGroup(UserListElements users);
	int getEntriesCount(const QList<UserListElements> &uids, HistoryEntryType type);
	void deleteHistory(const UserListElements &users);
	/**
		\fn void appendMessageEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje wiadomo¶ci w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendMessageEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	/**
		\fn void appendSmsEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje smsy w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendSmsEntry(UserListElements list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	/**
		\fn void appendStatusEntry(UserListElements list, const QString &status, const QString &desc, time_t time, const QString &adds);
		Zapisuje statusy w bazie. Z za³o¿enia interfejs dla modu³ów importu/eksportu historii.
		\param list lista u¿ytkowników
		\param msg tre¶æ wiadomo¶ci
		\param outgoing wiadomo¶æ wys³ana
		\param send_time czas wys³ania wiadomo¶ci
		\param receive_time czas odebrania 
	**/
	void appendStatusEntry(UserListElements list, const QString &status, const QString &desc, time_t time, const QString &adds);

	/**
		\fn void removeHistory(const UserListElements& uids, const QDate &date = 0);
		Funkcja usuwa ca³± historiê dla podanej listy u¿ytkowników lub tylko dla podanej daty.
		\param uids lista u¿ytkowników
	**/
	void removeHistory(const UserListElements& uids, const QDate &date = QDate(), HistoryEntryType type = EntryTypeMessage);
		
	bool beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();

	void addMenuActionDescription(ActionDescription *actionDescription);
	void insertMenuActionDescription(int pos, ActionDescription *actionDescription);
	void removeMenuActionDescription(ActionDescription *actionDescription);
	QAction * addMenuSeparator();
	void removeMenuSeparator(QAction *separator);
	virtual HistoryDlg* getHistoryDialog() { return historyDialog;};

public slots:
	void appendSms(const QString &mobile, const QString &msg);
	void appendStatus(UserListElement elem, QString protocolName);
	void chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time);
	void messageSentAndConfirmed(UserListElements, const QString&, time_t time = 0);
	HistorySearchResult searchHistory(UserListElements users, HistorySearchParameters params);

};

extern SqlHistory* sql_history;

#endif
