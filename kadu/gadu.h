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
#include <qlistbox.h>

#include "libgadu.h"

typedef uin_t UinType;

class UserList;
class UserListElement;

extern QHostAddress config_extip;

// ------------------------------------
//              Timers
// ------------------------------------

class AutoConnectionTimer : private QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();

	public slots:
		void doConnect();

	private:
		AutoConnectionTimer(QObject *parent = 0, const char *name=0);

		static AutoConnectionTimer *autoconnection_object;
};

// ------------------------------------
//            UinsList
// ------------------------------------

class UinsList : public QValueList<UinType>
{
	public:
		UinsList();
		bool equals(const UinsList &uins) const;
		void sort();
};

// ------------------------------------
//              Search
// ------------------------------------

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

// ------------------------------------
//             Notifiers
// ------------------------------------

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

// ------------------------------------
//             Status
// ------------------------------------

enum eStatus
{
	Online,
	Busy,
	Invisible,
	Offline
};

class Status : public QObject
{
	Q_OBJECT

	private:
		bool Changed;

	protected:
		eStatus Stat;
		QString Description;
		bool FriendsOnly;

	public:
		Status();
		virtual ~Status();

		bool isOnline();
		bool isBusy();
		bool isInvisible();
		bool isOffline();
		static bool isOffline(int);
		bool hasDescription();
		bool isFriendsOnly();
		QString description();

		int getIndex();
		static int getIndex(eStatus, bool);

		virtual QPixmap getPixmap();
		virtual QPixmap getPixmap(eStatus, bool) = 0;

		static eStatus fromString(const QString &);
		static QString toString(eStatus, bool);

		static int getCount();
		static int getInitCount();
		static QString getName(int);

	public slots:
		void setOnline(const QString & = "");
		void setBusy(const QString & = "");
		void setInvisible(const QString & = "");
		void setOffline(const QString & = "");
		void setDescription(const QString & = "");
		void setStatus(const Status &);
		void setStatus(eStatus, const QString & = "");
		void setIndex(int, const QString & = "");
		void setFriendsOnly(bool);

	signals:
		void goOnline(const QString &);
		void goBusy(const QString &);
		void goInvisible(const QString &);
		void goOffline(const QString &);
};

class GaduStatus : public Status
{
	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		virtual QPixmap getPixmap(eStatus, bool);
		int getStatusNumber();
};

// ------------------------------------
//            GaduProtocol
// ------------------------------------

class GaduProtocol : public QObject
{
	Q_OBJECT

	private:
		static QValueList<QHostAddress> ConfigServers;

		struct gg_login_params LoginParams;
		gg_session* Sess;

		GaduStatus *CurrentStatus;
		GaduStatus *NextStatus;

		GaduSocketNotifiers *SocketNotifiers;
		QTimer* PingTimer;

		unsigned int ServerNr;

		bool UserListSent;
		bool UserListClear;
		QString ImportReply;
		int RequestedStatusForLogin;
		bool IWannaBeInvisible;
		QHostAddress* ActiveServer;

		void setupProxy();

		void login();
		void logout();

	private slots:
		void registerDone(bool ok, struct gg_http *);
		void unregisterDone(bool ok, struct gg_http *);
		void remindDone(bool ok, struct gg_http *);
		void changePasswordDone(bool ok, struct gg_http *);

		void connectedSlot();
		void disconnectedSlot();
		void connectionTimeoutTimerSlot();
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

		void iWantGoOnline(const QString &);
		void iWantGoBusy(const QString &);
		void iWantGoInvisible(const QString &);
		void iWantGoOffline(const QString &);

	public:
		static void initModule();
		GaduProtocol(QObject *parent=NULL, const char *name=NULL);
		virtual ~GaduProtocol();

		Status & status();

		/**
			Zwraca serwer z ktorym jestesmy polaczeni
			lub do ktorego sie wlasnie laczymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

		/**
			Zamienia list�uytkownik� na a�uch i na odwr�
		**/
		QString userListToString(const UserList& userList) const;
		void stringToUserList(QString&, UserList& userList) const;
		void streamToUserList(QTextStream&, UserList& userList) const;

		void enableAutoConnection();
		void disableAutoConnection();

//		StatusType getCurrentStatus();
		void blockUser(const UinType&, bool);
		void offlineToUser(const UinType&, bool);
		void addNotify(const UinType&);
		void removeNotify(const UinType&);
		void addNotifyEx(const UinType&, bool blocked, bool offline);
		void removeNotifyEx(const UinType &, bool blocked, bool offline);

		bool userListSent();

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
			Wysya prob�o przysanie obrazka.
		**/
		bool sendImageRequest(UinType uin,int size,uint32_t crc32);
		bool sendImage(UinType uin,const QString& file_name,uint32_t size,char* data);


		/**
		 	Rejestruje nowego uytkownika
		**/
		bool doRegister(QString& mail, QString& password, QString& token_id, QString& token_val);

		/**
		 	Wyrejestrowuje uytkownika
		**/
		bool doUnregister(UinType uin, QString& password, QString& token_id, QString& token_val);

		/**
		  	Przypomina haso
		**/
		bool doRemind(UinType uin, QString& token_id, QString& token_val);

		/**
		  	Zmienia haso
		**/
		bool doChangePassword(UinType uin, QString& mail, QString& password, QString& new_password, QString& token_id, QString& token_val);

		/**
			Wysya userlist na serwer
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

		/**
			Zmieniamy sobie status
		**/
		//void setStatus(StatusType status);

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

		// to raczej chwilowo
		void freeEvent(struct gg_event* e);

		// --------------------
		//  DCC
		// --------------------

		void dccRequest(UinType);
		void dccFree(struct gg_dcc* d);
		struct gg_event* dccWatchFd(struct gg_dcc* d);
		void dccSetType(struct gg_dcc* d, int type);
		int dccFillFileInfo(struct gg_dcc* d, const QString& filename);
		struct gg_dcc* dccSocketCreate(UinType uin, uint16_t port);
		struct gg_dcc* dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		struct gg_dcc* dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		struct gg_dcc* dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		int dccVoiceSend(struct gg_dcc* d, char* buf, int length);
		void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

		// przeniesione z events.h
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void ifDefServerEnabled(bool value);
		void useTlsEnabled(bool value);
		void _Left();
		void _Right();
		void _Left2(QListBoxItem *item);
		void _Right2(QListBoxItem *item);
		void ifNotifyGlobal(bool toggled);
		void ifNotifyAll(bool toggled);

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
		void dccConnectionReceived(const UserListElement&);
		void disconnectNetwork();
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);
		void registered(bool ok, UinType uin);
		void unregistered(bool ok);
		void reminded(bool ok);
		void passwordChanged(bool ok);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void userListImported(bool ok, UserList&);

		void goOnline(const QString &);
		void goBusy(const QString &);
		void goInvisible(const QString &);
		void goOffline(const QString &);

		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora przyszla z serwera jeszcze w jej oryginalnej
			formie przed konwersja na unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic grzebiac w buforze msg,
			ale uwaga: mona zepsu�formatowanie tekstu zapisane
			w formats. Oczywicie je r�nie mona zmienia� wedug
			opisu protokou GG ;)
			Mozna tez przerwac dalsza obrobke wiadomoci ustawiajac
			stop na true.
		**/
		void messageFiltering(const UinsList& senders,QCString& msg,
			QByteArray& formats,bool& stop);
		/**
			Otrzymano wiadomo� kt� trzeba pokaza�(klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow sygnau chatMsgReceived1 ustawi zmienna
			grab na true to sygnal chatReceived2 nie zostanie wygenerowany.
			Jeli natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to adna czynno�zwizana z obsug tego
			zdarzenia nie zostanie podj�a (tj. wywietlanie wiadomoci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomoci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);

};

extern GaduProtocol* gadu;

#endif

