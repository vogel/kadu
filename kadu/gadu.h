#ifndef GADU_H
#define GADU_H

#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qvaluelist.h>

#include "libgadu.h"

class UinsList;
class UserList;

class QSocketNotifier;
class QTextStream;

extern struct gg_session* sess;
extern struct gg_login_params loginparams;

extern QSocketNotifier* kadusnr;
extern QSocketNotifier* kadusnw;

extern bool userlist_sent;
extern bool socket_active;
extern int last_read_event;
extern unsigned int server_nr;
extern bool timeout_connected;
extern QTimer* pingtimer;
extern QValueList<QHostAddress> config_servers;
extern bool i_wanna_be_invisible;

struct SearchResult
{
	QString Uin;
	QString First;
	QString Last;
	QString Nick;
	QString Born;
	QString City;
	QString FamilyName;
	QString FamilyCity;
	int Gender;
	int Status;

	SearchResult();
	SearchResult(const SearchResult&);
	void setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
		const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status);
};

typedef QValueList<SearchResult> SearchResults;

struct SearchRecord
{
	int Seq;
	int FromUin;
	QString Uin;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString City;
	QString BirthYearFrom;
	QString BirthYearTo;
	int Gender;
	bool Active;

	SearchRecord();
	virtual ~SearchRecord();

	void reqUin(const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();
};

class SocketNotifiers : public QObject
{
	Q_OBJECT

	protected:
		int Fd;
		QSocketNotifier *Snr;
		QSocketNotifier *Snw;

		void createSocketNotifiers();
		void deleteSocketNotifiers();
		void recreateSocketNotifiers();
		virtual void socketEvent() = 0;

	protected slots:
		virtual void dataReceived() = 0;
		virtual void dataSent() = 0;

	public:
		SocketNotifiers(int);
		virtual ~SocketNotifiers();
		void start();
};

class PubdirSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_http *H;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		PubdirSocketNotifiers(struct gg_http *);
		virtual ~PubdirSocketNotifiers();

	signals:
		void done(bool ok, struct gg_http *);
};

class DccSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_dcc *D;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		DccSocketNotifiers(struct gg_dcc *);
		virtual ~DccSocketNotifiers();

};

typedef uin_t UinType;

class GaduProtocol : public QObject
{
	Q_OBJECT

	private:
		bool userListClear;
		QString importReply;

	private slots:
		void registerDone(bool ok, struct gg_http *);
		void unregisterDone(bool ok, struct gg_http *);
		void remindDone(bool ok, struct gg_http *);
		void changePasswordDone(bool ok, struct gg_http *);

	public:	
		static void initModule();
		GaduProtocol();
		virtual ~GaduProtocol();

		/**
			Zamienia listê u¿ytkowników na ³añcuch i na odwrót
		**/
		QString userListToString(const UserList& userList) const;
		void stringToUserList(QString&, UserList& userList) const;
		void streamToUserList(QTextStream&, UserList& userList) const;

		/**
			Wysyla wiadomosc. bez formatowania tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessage(const UinsList& uins,const char* msg);
		/**
			Wysyla wiadomosc z formatowaniem tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessageRichText(const UinsList& uins,const char* msg,unsigned char* myLastFormats,int myLastFormatsLength);
		/**
			Wysy³a pro¶bê o przys³anie obrazka.
		**/
		bool sendImageRequest(UinType uin,int size,uint32_t crc32);
		bool sendImage(UinType uin,const QString& file_name,uint32_t size,char* data);

		/**
		  	Szuka ludzi w katalogu publicznym
		**/
		void searchInPubdir(SearchRecord& searchRecord);
		void searchNextInPubdir(SearchRecord& searchRecord);

		/**
		 	Rejestruje nowego u¿ytkownika
		**/
		bool doRegister(QString& mail, QString& password, QString& token_id, QString& token_val);

		/**
		 	Wyrejestrowuje u¿ytkownika
		**/
		bool doUnregister(UinType uin, QString& password, QString& token_id, QString& token_val);

		/**
		  	Przypomina has³o
		**/
		bool doRemind(UinType uin, QString& token_id, QString& token_val);

		/**
		  	Zmienia has³o
		**/
		bool doChangePassword(UinType uin, QString& mail, QString& password, QString& new_password, QString& token_id, QString& token_val);

		/**
			Wysy³a userlist na serwer
		**/
		bool doExportUserList(const UserList &);

		/**
			Czyscli userliste na serwerze
		**/
		bool doClearUserList();

		/**
			Importuje liste z serwera
		**/
		bool doImportUserList();

		/**
			Pobiera informacje o danych odobowych z katalogu publicznego
		**/
		void getPersonalInfo(SearchRecord& searchRecord);

		/**
			Ustawia informacje o danych osobowych z katalogu publicznego
		**/
		void setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData);

	private slots:
		void newResults(gg_pubdir50_t res);
		void userListReplyReceived(char, char *);

	public slots:
		void sendUserList();

	signals:
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);
		void registered(bool ok, UinType uin);
		void unregistered(bool ok);
		void reminded(bool ok);
		void passwordChanged(bool ok);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void userListImported(bool ok, UserList&);
};

extern GaduProtocol* gadu;

#endif

