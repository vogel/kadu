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
#include "status.h"

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
//             Status
// ------------------------------------

class GaduStatus : public Status
{
	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		void operator = (const Status &copyMe);

		virtual QPixmap pixmap(eStatus status, bool has_desc, bool mobile) const;

		int toStatusNumber() const;
		static int toStatusNumber(eStatus status, bool has_desc);

		void fromStatusNumber(int statusNumber, const QString &description);
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
	GaduStatus Stat;

	SearchResult();
	SearchResult(const SearchResult &);
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
		virtual void start();
		virtual void stop();
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

class TokenSocketNotifiers : public SocketNotifiers
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
		TokenSocketNotifiers();
		virtual ~TokenSocketNotifiers();

		virtual void start();

	signals:
		void gotToken(QString, QPixmap);
		void tokenError();
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
		void userlistReceived(const struct gg_event *);
		void userlistReplyReceived(char, char *);
		void userStatusChanged(const struct gg_event *);
};

// ------------------------------------
//            GaduProtocol
// ------------------------------------

class GaduProtocol : public QObject
{
	Q_OBJECT

	friend class AutoConnectionTimer;

	private:

		enum {
			Register,
			Unregister,
			RemindPassword,
			ChangePassword
		} Mode;

		UinType DataUin;
		QString DataEmail;
		QString DataPassword;
		QString DataNewPassword;
		QString TokenId;
		QString TokenValue;

		static QValueList<QHostAddress> ConfigServers;

		struct gg_login_params LoginParams;
		gg_session* Sess;

		/**
			rzeczywisty bie¿±cy status
		**/
		GaduStatus* CurrentStatus;
		/**
			status docelowy, który niekoniecznie zosta³
			jeszcze przyjêty przez serwer
		**/
		GaduStatus* NextStatus;

		GaduSocketNotifiers *SocketNotifiers;
		QTimer* PingTimer;

		unsigned int ServerNr;

		bool UserListSent;
		bool UserListClear;
		QString ImportReply;
		int RequestedStatusForLogin;
		QHostAddress* ActiveServer;

		void setupProxy();

		void login();
		void logout();

		void getToken();

		void doRegisterAccount();
		void doUnregisterAccount();
		void doRemindPassword();
		void doChangePassword();

	private slots:
		void registerDone(bool ok, struct gg_http *);
		void unregisterDone(bool ok, struct gg_http *);
		void remindDone(bool ok, struct gg_http *);
		void changePasswordDone(bool ok, struct gg_http *);

		void tokenError();
		void gotToken(QString, QPixmap);

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
		void userListReceived(const struct gg_event *);
		void userListReplyReceived(char, char *);
		void userStatusChanged(const struct gg_event *);

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
			Zwraca serwer z którym jeste¶my po³±czeni
			lub do którego siê w³a¶nie ³±czymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

		/**
			Zamienia listê u¿ytkowników na ³añcuch i na odwrót
		**/
		QString userListToString(const UserList& userList) const;
		void stringToUserList(QString&, UserList& userList) const;
		void streamToUserList(QTextStream&, UserList& userList) const;

		void enableAutoConnection();
		void disableAutoConnection();

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
			Wysya probï¿½o przysanie obrazka.
		**/
		bool sendImageRequest(UinType uin,int size,uint32_t crc32);
		bool sendImage(UinType uin,const QString& file_name,uint32_t size,char* data);

		/**
			Zarz±dzanie kontem
		**/
		void registerAccount(const QString &mail, const QString &password);
		void unregisterAccount(UinType uin, const QString &password);
		void remindPassword(UinType uin);
		void changePassword(UinType uin, const QString &mail, const QString &password,
			const QString &newPassword);

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

		// userlist
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);

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
		void userStatusChanged(const UserListElement &, const Status &oldStatus, bool onConnection = false);
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

		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora przyszla z serwera jeszcze w jej oryginalnej
			formie przed konwersja na unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic grzebiac w buforze msg,
			ale uwaga: mona zepsuï¿½formatowanie tekstu zapisane
			w formats. Oczywicie je rï¿½nie mona zmieniaï¿½ wedug
			opisu protokou GG ;)
			Mozna tez przerwac dalsza obrobke wiadomoci ustawiajac
			stop na true.
		**/
		void messageFiltering(const UinsList& senders,QCString& msg,
			QByteArray& formats,bool& stop);
		/**
			Otrzymano wiadomoï¿½ ktï¿½ trzeba pokazaï¿½(klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Treï¿½zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow sygnau chatMsgReceived1 ustawi zmienna
			grab na true to sygnal chatReceived2 nie zostanie wygenerowany.
			Jeli natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to adna czynnoï¿½zwizana z obsug tego
			zdarzenia nie zostanie podjï¿½a (tj. wywietlanie wiadomoci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomoci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);

		/**
			Wywo³ywane, gdy chcemy odczytaæ token z obrazka
		**/
		void needTokenValue(QPixmap, QString &);

};

extern GaduProtocol* gadu;

#endif
