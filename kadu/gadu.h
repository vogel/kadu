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
		/**
			konstruuje obiekt UinsList
		**/
		UinsList();
		
		/**
			konstruuje obiekt UinsList na podstawie ³añcucha "uins" sk³adaj±cego
			siê z oddzielonych przecinkami Uinów
		**/
		UinsList(const QString &uins);

		/**
			konstruuje obiekt UinsList na podstawie "list"
			przekszta³acaj±c ka¿dy element do typy UinType
		**/
		UinsList(const QStringList &list);

		bool equals(const UinsList &uins) const;
		void sort();
		QStringList toStringList() const;
};

// ------------------------------------
//             UserStatus
// ------------------------------------

class GaduStatus : public UserStatus
{
	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		void operator = (const UserStatus &copyMe);

		virtual QPixmap pixmap(eUserStatus status, bool has_desc, bool mobile) const;

		int toStatusNumber() const;
		static int toStatusNumber(eUserStatus status, bool has_desc);

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
		SocketNotifiers(int fd, QObject *parent=0, const char *name=0);
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
		PubdirSocketNotifiers(struct gg_http *, QObject *parent=0, const char *name=0);
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
		TokenSocketNotifiers(QObject *parent=0, const char *name=0);
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
		GaduSocketNotifiers(QObject *parent=0, const char *name=0);
		virtual ~GaduSocketNotifiers();
		void setSession(gg_session *sess);
		void checkWrite();

	signals:
		void ackReceived(int seq, uin_t uin, int status);
		void connected();
		/**
			Otrzymano wiadomo¶æ CTCP.
			Kto¶ nas prosi o po³±czenie dcc, poniewa¿
			jeste¶my za NAT-em.
			TODO: zmieniæ nazwê.
		**/
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

/**
	@class GaduProtocol
	@short Klasa do obs³ugi protoko³u Gadu-Gadu
 **/
class GaduProtocol : public QObject
{
	Q_OBJECT

	friend class AutoConnectionTimer;

	private:

		/**
			@enum Mode
			@short Tryb pracy obiektu
			@see TokenSocketNotifiers
			@see gotToken
			@see needTokenValue
			@see register
			@see doRegister
			@see unregister
			@see doUnregister
			@see remindPassword
			@see doRemindPassword
			@see changePassword
			@see doChangePassword

			Kilka operacja w protokole Gadu-Gadu wymaga od klienta wys³ania do serwera, oprócz
			standardowych pakietów, tak zwanego 'tokena'. Najpierw pobierany jest z serwera identyfikator
			tokena oraz obrazek. Nastêpnie z obrazka odczytywana jest warto¶c tokena (w za³o¿eniach
			mia³o to zapobiegaæ rejestrowaniu nowych u¿ytkowników przez automaty.

			Sloty register, unregister, remindPassword i changePassword inicjuj± pobieranie tokena
			i ustalaj± warto¶æ pola Mode. Pobieranie obrazka realizowane jest przez klasê
			TokenSocketNotifiers. Po pobraniu wywo³ywany jest slot gotToken, który na podstawie warto¶ci
			pola Mode wywo³uje jedn± z funkjci doRegister, doUnregister, doRemindPassword i doChangePassword.
		 **/
		enum
		{
			//! Rejestrowanie nowego u¿ytkownika
			Register,
			//! Wyrejestrowywanie istniej±cego u¿ytkownika
			Unregister,
			//! Przypominanie has³a
			RemindPassword,
			//! Zmienianie has³a
			ChangePassword
		} Mode;

		//! Identyfikator u¿ytkownika
		UinType DataUin;
		//! e-mail u¿ytkownika
		QString DataEmail;
		//! stare has³o u¿ytkownika
		QString DataPassword;
		//! nowe has³o u¿ytkownika
		QString DataNewPassword;
		//! identyfikator tokena
		QString TokenId;
		//! warto¶æ tokena
		QString TokenValue;

		//! Serwery, z którymi ³aczy siê obiekt.
		static QValueList<QHostAddress> ConfigServers;
		//! Numer serwera, do którego obiekt ostatnio próbowa³ siê pod³±czyæ.
		unsigned int ServerNr;
		//! Adres serwera, do którego obiekt jest pod³±czony.
		QHostAddress* ActiveServer;

		//! Parametry logowania - wymagane przez bibliotekê libgg
		struct gg_login_params LoginParams;
		//! Sesja po³±czenia - wymagane przez bibliotekê libgg
		gg_session* Sess;

		/**
			Bie¿acy status. Zmieniany po po³±czeniu, oraz w przypadku zmiany statusu kiedy po³±czenie
			jest ju¿ zainicjowane.

			@see login
			@see connected
			@see iWantGoOnline
			@see iWantGoBusy
			@see iWantGoInvisible
			@see iWantGoOffline
			@see NextStatus
			@see UserStatus
			@see status
		**/
		GaduStatus* CurrentStatus;

		/**
			Nastêpny status. Ustalany zewnêtrznie przy wykorzystaniu metody status i odpowiednich
			slotów klasy UserStatus. Zmiana wywo³uje jedn± z metod iWantGo... i w konsekwencji zmianê
			statusu (w razie konieczno¶ci te¿ zalogowanie).

			@see login
			@see connected
			@see iWantGoOnline
			@see iWantGoBusy
			@see iWantGoInvisible
			@see iWantGoOffline
			@see CurrentStatus
			@see UserStatus
			@see status
		 **/
		GaduStatus* NextStatus;

		/**
			Klasa gniazdek ³±cz±ca siê z serwerem. Wysy³a sygna³y po wyst±pieniu zdarzenia protoko³u
			(po³±czenie, zerwanie po³±czenia, nowa wiadomo¶æ).

			@see GaduSocketNotifiers
		 **/
		GaduSocketNotifiers *SocketNotifiers;

		/**
			Zegar pinguj±cy serwer.
		 **/
		QTimer* PingTimer;

		/**
			Okre¶la, czy lista u¿ytkowników zosta³a ju¿ wys³ana.

			@todo Wywaliæ, zamieniæ na connected
		 **/
		bool UserListSent;
		/**
			Zmienna ustawiana w zale¿no¶ci od tego, czy wysy³amy listê kontaktów na serwer
			czy te¿ usuwamy j± z tego serwera. Zakoñczenie obydwu tych czynno¶ci wywo³uje
			sygna³ podpiêty do slotu userListReplyReceived, który w zale¿no¶ci od warto¶ci
			tego pola wywo³uje userListCleared albo userListExported.

			@see userListReplyReceived
			@see userListCleared
			@see userListExported
		 **/
		bool UserListClear;

		/**
			Lista u¿ytkowników pobrana z serwera w postaci ³añcucha. Warto¶c ustalana w slocie
			userListReplyReceived.

			@see userListReplyReceived
		 **/
		QString ImportReply;

		/**
			Ustawianie parametrów po³±czenia proxy. Metoda wywo³ywana podczas logowania.

			@see login
		 **/
		void setupProxy();

		/**
			Loguje siê do serwera Gadu-Gadu. Po uruchomieniu emituje sygna³ connecting. Parametry
			logowania odczytuje z konfiguracji, status logowania pobiera z pola NextStatus.
			£±cz±c siê, wybiera kolejne serwery (w przypadku nieudanego po³±czenia) wykorzystuj±c
			pola ConfigServers i i ServerNr.

			Po poprawnym zalogowaniu wywo³ywany jest slot connectedSlot, który emituje sygna³
			GaduProtocol::connected

			Metodê mo¿na uruchomiæ po¶rednio poprzez wywo³anie typu gadu->status().setOnline(),
			które wywo³a slot iWantToGoOnline, który z kolei (gdy stwierdzi, ¿e nie jeste¶my zalogowani)
			wywo³a procedurê.

			Metoda jest te¿ wywo³ywana przez obiekt statyczny klasy AutoConnectionTimer, która
			jest zaprzyja¼niona z GaduProtocol.

			@see connecting
			@see connected
			@see connectedSlot
			@see NextStatus
			@see ConfigServers
			@see ServerNr
			@see AutoConnectionTimer
		 **/
		void login();
		/**
			Metoda wywo³ywana w razie roz³±czenie siê z serwerem. Wywo³ywana przez iWantGoOffline
			albo przez connectionTimeoutTimerSlot. Wywo³uje disconnectedSlot, który z kolei
			emituje sygna³ disconnected

			@see disconnected
			@see disconnectedSlot
			@see connectionTimeoutTimerSlot
			@see iWantGoOffline
			@todo Wywaliæ i zast±piæ wywo³aniami disconnectedSlot?
		 **/
		void logout();

		/**
			Za pomoc± klasy TokenSocketNotifiers metoda pobiera z serwera GaduGadu token wraz
			z identyfikatorem. Pobrany token jest obs³ugiwany za pomoc± slota gotToken,
			który pobiera warto¶æ tokena emituj±c sygna³ needTokenValue i nastêpnie wywo³uj±c
			jedn± z metod  doRegisterAccount, doUnregisterAccount(), doRemindPassword(),
			doChangePassword() na podstawie warto¶ci pola Mode.

			@see TokenSocketNotifiers
			@see gotToken
			@see doRegisterAccount
			@see doUnregisterAccount
			@see doRemindPassword
			@see doChangePassword
			@see Mode
		 **/
		void getToken();

		/**
			Rejestruje nowe konto. Wywo³ywane przez gotToken (które jest wywo³ane po¶rednio przez
			registerAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu registerDone,
			który emituje sygna³ registered.

			@see registerAccount
			@see registered
			@see registerDone
			@see gotToken
		 **/
		void doRegisterAccount();
		/**
			Wyrejestrowuje konto. Wywo³ywane przez gotToken (które jest wywo³ane po¶rednio przez
			unregisterAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu unregisterDone,
			który emituje sygna³ unregistered.

			@see unregisterAccount
			@see unregistered
			@see unregisterDone
			@see gotToken
		 **/
		void doUnregisterAccount();
		/**
			Przypomina has³o. Wywo³ywane przez gotToken (które jest wywo³ane po¶rednio przez
			remindPassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu remindDone,
			który emituje sygna³ reminded.

			@see remindPassword
			@see reminded
			@see remindDone
			@see gotToken
		 **/
		void doRemindPassword();
		/**
			Zmienia has³o. Wywo³ywane przez gotToken (które jest wywo³ane po¶rednio przez
			changePassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu changePasswordDone,
			który emituje sygna³ passwordChanged.

			@see changePassword
			@see passwordChanged
			@see changePasswordDone
			@see gotToken
		 **/
		void doChangePassword();

	private slots:
		/**
			Wywo³ywany po zarejestrowaniu konta. Emituje registered/

			@see registerAccount
			@see doRegisterAccount
			@see registered
		 **/
		void registerDone(bool ok, struct gg_http *);
		/**
			Wywo³ywany po wyrejestrowaniu konta. Emituje unregistered.

			@see unregisterAccount
			@see doUnregisterAccount
			@see unregistered
		 **/
		void unregisterDone(bool ok, struct gg_http *);
		/**
			Wywo³ywany po przypomnieniu has³a. Emituje reminded.

			@see remindPassword
			@see doRemindPassword
			@see reminded
		 **/
		void remindDone(bool ok, struct gg_http *);
		/**
			Wywo³ywany po zmianie has³a. Emituje passwordChanged.

			@see changePassword
			@see doChangePassword
			@see passwordChanged
		 **/
		void changePasswordDone(bool ok, struct gg_http *);

		/**
			Slot wywo³ywany, gdy pobieranie tokena siê nie uda³o.

			@see getToken
		**/
		void tokenError();
		/**
			Slot wywo³ywany, gdy pobieranie tokena siê powiod³o. Emituje needTokenValue

			@see getToken
			@see needTokenValue
		 **/
		void gotToken(QString, QPixmap);

		/**
			Slot wywo³ywany po po³±czeniu z serwerem. Emituje connected i w³±cza pingowanie
			serwera.

			@see connected
		 **/
		void connectedSlot();
		/**
			Slot wywo³ywany po roz³±czeniu z serwerem. Emituje disconnected i wy³±cza pingowanie
			serwera.

			@see disconnected
		 **/
		void disconnectedSlot();
		/**
			Slot wywo³ywany po przekroczeniu czasu po³±czenia. Próbuje po³aczyæ ponownie.
		 **/
		void connectionTimeoutTimerSlot();
		/**
			Slot wywo³ywane po wyst±pieniu b³êdu po³±czenia. Emituje disconnected i error.

			@see error
			@see disconnected
		 **/
		void errorSlot(GaduError);
		/**
			Slot wywo³ywany po otrzymaniu obrazka od serwera. Emituje imageReceivedAndSaved

			@see imageReceivedAndSaved
		 **/
		void imageReceived(UinType sender, uint32_t size, uint32_t crc32,
			const QString &filename, const char *data);
		/**
			Slot wywo³ywany po otrzymaniu pro¶by o obrazek od serwera. Wysy³a obrazek.
		 **/
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		/**
			Slot wywo³ywany po otrzymaniu wiadomo¶ci od serwera.
		 **/
		void messageReceived(int, UinsList, QCString& msg, time_t, QByteArray &formats);
		/**
			Co pewien czas pinguje serwer.
		 **/
		void pingNetwork();
		/**
			Obs³uguje otrzymanie nowych wyników wyszukiwania z serwera. Emituje newSearchResults.

			@see newSearchResults
		 **/
		void newResults(gg_pubdir50_t res);
		/**
			Nowa wiadomo¶æ od serwera. Emituje systemMessageReceived

			@see systemMessageReceived
		 **/
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		/**
			Pobrano listê u¿ytkowników z serwera. Emituje userStatusChanged dla ka¿dego
			otrzymanego kontaktu oraz userListChanged przed zakoñczeniem dzia³ania.

			@see userStatusChanged
			@see userListChanged
		 **/
		void userListReceived(const struct gg_event *);
		/**
			Odpowied¼ od serwera na temat operacji na li¶cie u¿ytkowników. Emituje, w zale¿no¶ci
			od trybu dzia³ania: userListCleared, userListExported, userListImported.

			@see userListCleared
			@see userListExported
			@see userListImported
		 **/
		void userListReplyReceived(char, char *);
		/**
			Informacja o zmianie statusu kontaktu. Emituje userStatusChanged oraz userListChanged.

			@see userStatusChanged
			@see userListChanged
		 **/
		void userStatusChanged(const struct gg_event *);

		/**
			Kto¶ wykona³ gadu.status().setOnline(). £±czymy z serwerem, je¿eli jeszcze tego nie
			zrobili¶my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		 **/
		void iWantGoOnline(const QString &);
		/**
			Kto¶ wykona³ gadu.status().setBusy(). £±czymy z serwerem, je¿eli jeszcze tego nie
			zrobili¶my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		 **/
		void iWantGoBusy(const QString &);
		/**
			Kto¶ wykona³ gadu.status().setInvisible(). £±czymy z serwerem, je¿eli jeszcze tego nie
			zrobili¶my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		 **/
		void iWantGoInvisible(const QString &);
		/**
			Kto¶ wykona³ gadu.status().setOffline(). Roz³±czamy siê z serwerem i ustawiamy opis (je¿eli
			byli¶my po³±czeni).

			@see CurrentStatus
			@see NextStatus
			@see logout
		 **/
		void iWantGoOffline(const QString &);
		
		/**
			Przysz³a informacja o dostarczeniu (lub nie) wiadomo¶ci.
			Na podstawie statusu emituje odpowiednie sygna³y message*

			@see messageBlocked
			@see messageDelivered
			@see messageQueued
			@see messageBoxFull
			@see messageNotDelivered
			@see messageAccepted
			@see messageRejected
		**/
		void ackReceived(int seq, uin_t uin, int status);
		
	public:
		static void initModule();
		GaduProtocol(QObject *parent=NULL, const char *name=NULL);
		virtual ~GaduProtocol();

		/**
			Status u¿ytkownika. Za pomoc± tej metody mo¿emy go zmieniæ, pobraæ ikonê statusu i wykonaæ
			kilka innych ciekawych rzeczy.

			1. Zmiena statusu:
			<code>
				GaduProtocol gadu;

				...

				gadu.status().setOnline("Jestem zalogowany"); // zalogowanie i ustawienie opisu
				gadu.status().setFriendsOnly(true);           // tryb tylko dla przyjació³
				...
				gadu.status().setOffline();                   // wylogowanie, usuniêcie opisu
			</code>

			2. Sprawdzenie statusu:
			<code>
				GaduProtocol gadu;

				if (gadu.status().isOnline())                 // jeste¶my online
					...
				else if (gadu.status().isInvisible())         // jeste¶my niewidzialni
					...

				// mo¿na te¿:
				switch (gadu.status().status())
				{
					case Online:
						break;
					case Busy:
						break;
					case Invisible:
						break;
					case Offline:
						break;
				}
			</code>

			3. Pobranie ikony i nazwy statusu
			<code>
				QPixmap pix;
				QString name;
				GaduProtocol gadu;

				...

				pix = gadu.status().pixmap();
				name = gadu.status().name();
			</code>

			@see currentStatus
		 **/
		UserStatus & status();

		/**
			Rzeczywisty aktualny status. Mo¿na go wykorzystaæ tylko w trybie do odczytu (pobranie
			ikony, nazwy, sprawdzenie rzeczywistego stanu po³aczenia).

			@see status
		 **/
		const UserStatus & currentStatus();

		/**
			Zwraca serwer z którym jeste¶my po³±czeni lub do którego siê w³a¶nie ³±czymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

		/**
			Konwertuje listê u¿ytkowników do postaci ³añcucha.

			£añcuch wynikowy ma postaæ:
			<code>
				opis_u¿ytkownika<CR><LF>
				opis_u¿ytkownika<CR><LF>
				...
				opis_u¿ytkownika<CR><LF>
			</code>

			opis_u¿ytkownika ma postaæ:
			<code>
				firstName;lastName;nickName;altNick;mobile;grupy;uin;email;0;;0;
			</code>

			grup maj± postaæ:
			<code>
				grupa_1,grupa_2,grupa_3
			</code>

			@param userList lista u¿ytkowników, która zostanie skonwertowana
			@return ³añcuch reprezentuj±cy listê u¿ytkowników
			@see stringToUserList
			@see streamToUserList
		 **/
		QString userListToString(const UserList &userList) const;
		/**
			Konwertujê ³añcuch do listy u¿ytkowników.

			Format ³añcucha jest anologiczny do tego z funkcji userListToString. Jedynym wyj±tkiem
			jest to, ¿e grupy oddzielone s± ¶rednikami, nie przecinkami.

			@param source ³añuch, bêd±cy reprezentacj± listy u¿ytkowników
			@param userList lista u¿ytkowników, do której zapisany zostanie wynik konwersji
			@see userListToString
			@see streamToUserList
		 **/
		void stringToUserList(QString &source, UserList &userList) const;
		/**
			Odczytuje ze strumienia ³añcuch reprezentuj±cy listê u¿ytkowników i konwertuje
			go go postaci obiektu UserList.

			Format ³añcucha jest anologiczny do tego z funkcji userListToString. Jedynym wyj±tkiem
			jest to, ¿e grupy oddzielone s± ¶rednikami, nie przecinkami.

			@param source strumieñ, z którego odczytane zostan± dane
			@param userList lista u¿ytkowników, do której zapisany zostanie wynik konwersji
			@see userListToString
			@see stringToUserList
		 **/
		void streamToUserList(QTextStream &source, UserList &userList) const;

		/**
			W³±cza próby automatycznego ³±czenia w razie niepowodzenia.
		 **/
		void enableAutoConnection();
		/**
			Wy³±cza próby automatycznego ³±czenia w razie niepowodzenia.
		 **/
		void disableAutoConnection();

		/**
			Zwraca true, je¿eli jeste¶my po³±czenie z serwerem.

			@todo zmieniæ nazwê na connected
		 **/
		bool userListSent();

	public slots:
		/**
			Wysy³a wiadomo¶æ bez formatowania tekstu. Je¶li adresatów jest wiêcej ni¿ jeden, to wysy³ana
			jest wiadomo¶æ konferencyjna. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.

			@param uins lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param msg wiadomo¶æ, któr± wysy³amy - musi byæ podana w postaci cp1250
			@toto zmieniæ na sendMessage(const UinsList &, QString &) z wewnêtrzn± konwersj± na cp1250
		**/
		int sendMessage(const UinsList &uins, const char *msg);

		/**
			Wysy³a wiadomo¶æ z formatowaniem tekstu. Je¶li adresatów jest wiêcej ni¿ jeden, to wysy³ana
			jest wiadomo¶æ konferencyjna. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.

			@param uins lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param msg wiadomo¶æ, któr± wysy³amy - musi byæ podana w postaci cp1250
			@param myLastFormats formatowanie tekstu
			@param myLastFormatsLength ilo¶c znaczników formatuj±cych

			@toto zmieniæ na sendMessageRichText(const UinsList &, QString &, ...)
				z wewnêtrzn± konwersj± na cp1250 oraz z jakim¶ lepszym sposobem formatowania tekstu
		**/
		int sendMessageRichText(const UinsList &uins, const char *msg, unsigned char *myLastFormats,
		 	int myLastFormatsLength);

		/**
			Wysy³a pro¶bê o przys³anie obrazka z danymi parametrami.

			@param uin u¿ytkownik, od którego chcemy obrazek
			@param size rozmiar obrazka w bajtach
			@param crc32 crc32 pliku
			@todo powinno byæ sendImageRequest(uniqId uint32_t) - info o obrazku zapisywaæ gdzie¶ w ¶rodku
		 **/
		bool sendImageRequest(UinType uin, int size, uint32_t crc32);
		/**
			Wywy³a obrazek o podanych parametrach.

			@param uin u¿ytkownik, któremu wysy³amy obrazek
			@param file_name nazwa pliku obrazka
			@param size rozmiar obrazka w bajtach
			@param data zawarto¶æ pliku
			@toto usun±æ parametry size i data - mo¿emy to chyba sami wyznaczyæ
		 **/
		bool sendImage(UinType uin, const QString &file_name, uint32_t size, char *data);

		/**
			Rejetrujemy nowe konto. Odpowied¼ przychodzi poprzez sygna³ registered. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param mail nasz email, pole nieobowi±zkowe
			@param password nasze has³o, pole obowi±zkowe
			@see registered
			@see needTokenValue
			@see unregisterAccount
		**/
		void registerAccount(const QString &mail, const QString &password);
		/**
			Wyrejestrowujemy stare konto. Odpowied¼ przychodzi poprzez sygna³ unregistered. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param uin nasz uin
			@param password nasze has³o
			@toto parametr uin naprawdê potrzebny?
		 **/
		void unregisterAccount(UinType uin, const QString &password);
		/**
			Wysy³a has³o na email. Odpowied¼ przychodzi poprzez sygna³ reminded. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param uin nasz uin
			@toto parametr uin naprawdê potrzebny?
		 **/
		void remindPassword(UinType uin);
		/**
			Zmienia nasze has³o. Odpowied¼ przychodzi poprzez sygna³ passwordChanged. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param uin nasz uin
			@param mail nasz email, jaki podali¶my przy rejestracji
			@param password stare has³o
			@param newPassword nowe has³o
			@toto parametr uin naprawdê potrzebny?
		 **/
		void changePassword(UinType uin, const QString &mail, const QString &password,
			const QString &newPassword);

		/**
			Wysy³a listê u¿ytkowników na serwer. Odpowied¼ przychodzi przez sygna³ userListExported.

			@return false, je¿li operacja siê nie powiod³a
			@param userList lista do wys³ania
			@see userListExported
			@todo usun±æ warto¶æ zwracan±
		**/
		bool doExportUserList(const UserList &userList);

		/**
			Usuwa listê u¿ytkowników z serwera. Odpowied¼ przychodzi przez sygna³ userListCleared.

			@return false, je¿li operacja siê nie powiod³a
			@see userListCleared
			@todo usun±æ warto¶æ zwracan±
		**/
		bool doClearUserList();

		/**
			Importuje listê u¿ytkowników z serwera. Odpowied¼ przychodzi przez sygna³ userListImported.

			@return false, je¿li operacja siê nie powiod³a
			@see userListImported
			@todo usun±æ warto¶æ zwracan±
		**/
		bool doImportUserList();

		/**
			Wysy³a nasz± listê u¿ytkowników na serwer. Uwaga: nie ma to nic wspólnego z importem/eksportem.
		 **/
		void sendUserList();

		/**
			Szuka ludzi w katalogu publicznym. Wyniki przychodz± za pomoca sygna³u newSearchResults.

			@param searchRecord dane do szukania
			@see newSearchResults
			@see searchNextInPubdir
		**/
		void searchInPubdir(SearchRecord& searchRecord);
		/**
			Szuka ludzi w katalogu publicznym. Wyniki przychodz± za pomoca sygna³u newSearchResults.

			@param searchRecord dane do szukania
			@see newSearchResults
			@see searchInPubdir
		**/
		void searchNextInPubdir(SearchRecord& searchRecord);

		/**
			Pobiera informacje o danych odobowych z katalogu publicznego.

			@todo jak to w ogóle dzia³a, bo zapomnia³em??
		**/
		void getPersonalInfo(SearchRecord& searchRecord);

		/**
			Ustawia informacje o danych osobowych w katalogu publicznym.

			@todo jak to w ogóle dzia³a, bo zapomnia³em??
		**/
		void setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData);

		/**
			 To raczej chwilowo.

			 @todo usun±æ
			 @todo niech Adrian zrobi porzadek z tym dcc...
		 **/
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

		/**
			Sygna³ wywo³ywany, gdy zmieni³y siê dane kontaktu. Umo¿liwia poinformowanie serwera
			o zmianie naszych zaleceñ co do obs³ugi wiadomo¶ci od kontaktu.

			Przyk³ady:
			<code>
				GaduProtocol gadu;
				UserListElement old, new;

				...
				gadu.userDataChanged(NULL, &new);              // dodali¶my nowy kontakt

				old = new;
				new.setBlockng(true);                          // blokujemy
				gadu.userDataChanged(&old, &new);              // informujemy o tym serwer

				gadu.userDataChanged(new, NULL);               // usunêli¶my kontakt
				...
			</code>

			Powy¿sze kontrukcje nie s± jednak na ogó³ potrzebne, gdy¿ obiekty UserListElement
			wywo³uj± odpowieni sygna³ swoich UserList (konkretnie userDataChanged), który mo¿na
			podpi±æ bezpo¶rednio pod taki sam slot klasy GaduProtocol, dziêki czemu ka¿da
			zmiana zostanie automatycznie zauwa¿ona przez serwer.

			<code>
				GaduProtocol gadu;
				UserListElement *ule;
				UserList ul;

				...                                            // po³aczenie odpowiednich slotów
				ule = &ul.byAltNick("AltNick");
				ule->setBlocking(true);                        // i wszystko robi siê samo
			</code>

			@param oldData wska¼nik do starych danych kontatku
			@param newData wska¼nik do nowych danych konaktu
		 **/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);

	signals:
		/**
			wiadomo¶æ zosta³a zablokowana przez serwer
		**/
		void messageBlocked(int seq, UinType uin);
		
		/**
			wiadomo¶æ dostarczono
		**/
		void messageDelivered(int seq, UinType uin);
		
		/**
			wiadomo¶æ zakolejkowano
		**/
		void messageQueued(int seq, UinType uin);
		
		/**
			skrzynka odbiorcza na serwerze jest pe³na
		**/
		void messageBoxFull(int seq, UinType uin);
		
		/**
			wiadomo¶æ nie dostaczona (wystêpuje tylko przy CTCP)
		**/
		void messageNotDelivered(int seq, UinType uin);
		
		/**
			wiadomo¶æ zosta³a przyjêta przez serwer 
		**/
		void messageAccepted(int seq, UinType uin);
		/**
			wiadomo¶æ zosta³a odrzucona przez serwer 
		**/
		void messageRejected(int seq, UinType uin);
		
		/**
			otrzymano informacjê o potwierdzeniu wiadomo¶ci
			
			wkrótce ten sygna³ zostanie usuniêty
			proszê u¿ywaæ messageAccepted() i messageRejected() lub szczegó³owych odpowiedników
		**/
		void ackReceived(int seq);
		
		void connected();
		void connecting();
		void disconnected();
		void error(GaduError);
		/**
			Otrzymano dane obrazka i zapisano go do pliku.
		**/
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void userListChanged();
		void userStatusChanged(const UserListElement &, const UserStatus &oldStatus, bool onConnection = false);
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
			Otrzymano wiadomo¶æ któr± trzeba pokazaæ (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre¶æ zdeszyfrowana i zdekodowana do unicode.
			Je¶li który¶ ze slotów sygna³u chatMsgReceived1 ustawi zmienn±
			grab na true to sygna³ chatReceived2 nie zostanie wygenerowany.
			Je¶li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to ¿adna czynno¶æ zwi±zana z obs³ug± tego
			zdarzenia nie zostanie podjêta (tj. wy¶wietlanie wiadomo¶ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo¶ci z CP1250 na Unicode.
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
