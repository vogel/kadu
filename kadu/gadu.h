#ifndef GADU_H
#define GADU_H

#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qsocketnotifier.h>
#include <qtextstream.h>

#include "libgadu.h"

typedef uin_t UinType;

class UserList;
class UserListElement;

extern struct gg_session* sess;
extern struct gg_login_params loginparams;

extern bool userlist_sent;
extern bool socket_active;
extern unsigned int server_nr;
extern QValueList<QHostAddress> config_servers;

class UinsList : public QValueList<UinType>
{
	public:
		UinsList();
		bool equals(const UinsList &uins) const;
		void sort();
};

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
		void stop();
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

typedef enum
{
	ConnectionServerNotFound,
	ConnectionCannotConnect,
	ConnectionNeedEmail,
	ConnectionInvalidData,
	ConnectionCannotRead,
	ConnectionCannotWrite,
	ConnectionIncorrectPassword,
	ConnectionTlsError,
	ConnectionUnknow,
	ConnectionTimeout,
	Disconnected
} GaduError;

class GaduSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		gg_session *Sess;

		void connectionFailed(int);

	protected:
		virtual void socketEvent();

	public slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		GaduSocketNotifiers();
		virtual ~GaduSocketNotifiers();
		void setSession(gg_session *sess);
		void checkWrite();

	signals:
		void ackReceived(int);
		void connected();
		void dccConnectionReceived(const UserListElement &);
		void disconnected();
		void error(GaduError);
		void imageReceived(UinType, uint32_t, uint32_t, const QString &filename, const char *data);
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		void messageReceived(int, UinsList, QCString &, time_t, QByteArray &);
		void pubdirReplyReceived(gg_pubdir50_t);
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		void userlistReceived(struct gg_event *);
		void userlistReplyReceived(char, char *);
		void userStatusChanged(struct gg_event *);
};

class GaduProtocol : public QObject
{
	Q_OBJECT

	private:
		GaduSocketNotifiers *SocketNotifiers;
		QTimer* PingTimer;

		bool userListClear;
		QString importReply;
		int RequestedStatusForLogin;
		bool IWannaBeInvisible;
		QHostAddress* ActiveServer;

		void setupProxy();
		void setupDcc();
		void changeStatus(int status);

	private slots:
		void registerDone(bool ok, struct gg_http *);
		void unregisterDone(bool ok, struct gg_http *);
		void remindDone(bool ok, struct gg_http *);
		void changePasswordDone(bool ok, struct gg_http *);

		void connectedSlot();
		void disconnectedSlot();
		void connectionTimeoutTimerSlot();
		void dccConnectionReceived(const UserListElement &);
		void errorSlot(GaduError);
		void imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *data);
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		void messageReceived(int, UinsList, QCString& msg, time_t, QByteArray &formats);
		void pingNetwork();
		void newResults(gg_pubdir50_t res);
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		void userListReceived(struct gg_event *);
		void userListReplyReceived(char, char *);
		void userStatusChanged(struct gg_event *);

	public:
		static void initModule();
		GaduProtocol(QObject *parent=NULL, const char *name=NULL);
		virtual ~GaduProtocol();

		/**
			Zwraca serwer z ktorym jestesmy polaczeni
			lub do ktorego sie wlasnie laczymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

		/**
			Zamienia listê u¿ytkowników na ³añcuch i na odwrót
		**/
		QString userListToString(const UserList& userList) const;
		void stringToUserList(QString&, UserList& userList) const;
		void streamToUserList(QTextStream&, UserList& userList) const;

	public slots:
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

		void sendUserList();
		void login(int status);
		void logout();

		/**
			Zmieniamy sobie status
		**/
		void setStatus(int status);

		/**
		  	Szuka ludzi w katalogu publicznym
		**/
		void searchInPubdir(SearchRecord& searchRecord);
		void searchNextInPubdir(SearchRecord& searchRecord);

		/**
			Pobiera informacje o danych odobowych z katalogu publicznego
		**/
		void getPersonalInfo(SearchRecord& searchRecord);

		/**
			Ustawia informacje o danych osobowych z katalogu publicznego
		**/
		void setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData);

	signals:
		void ackReceived(int);
		void connected();
		void connecting();
		void disconnected();
		void error(GaduError);
		/**
			Otrzymano dane obrazka i zapisano go do pliku.
		**/
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void userListChanged();
		void userStatusChanged(UserListElement &, int oldstatus);
		void systemMessageReceived(QString &);

		void dccSetupFailed();
		void statusChanged(int);
		void disconnectNetwork();
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);
		void registered(bool ok, UinType uin);
		void unregistered(bool ok);
		void reminded(bool ok);
		void passwordChanged(bool ok);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void userListImported(bool ok, UserList&);

		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora przyszla z serwera jeszcze w jej oryginalnej
			formie przed konwersja na unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic grzebiac w buforze msg,
			ale uwaga: mo¿na zepsuæ formatowanie tekstu zapisane
			w formats. Oczywi¶cie je równie¿ mo¿na zmieniaæ, wed³ug
			opisu protoko³u GG ;)
			Mozna tez przerwac dalsza obrobke wiadomo¶ci ustawiajac
			stop na true.
		**/
		void messageFiltering(const UinsList& senders,QCString& msg,
			QByteArray& formats,bool& stop);
		/**
			Otrzymano wiadomo¶æ, któr± trzeba pokazaæ (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre¶æ zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow sygna³u chatMsgReceived1 ustawi zmienna
			grab na true to sygnal chatReceived2 nie zostanie wygenerowany.
			Je¶li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to ¿adna czynno¶æ zwi±zana z obs³ug± tego
			zdarzenia nie zostanie podjêta (tj. wy¶wietlanie wiadomo¶ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo¶ci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);

};

extern GaduProtocol* gadu;

#endif

