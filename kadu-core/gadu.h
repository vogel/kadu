#ifndef GADU_H
#define GADU_H

#include <qhostaddress.h>
#include <qvaluelist.h>

#include "libgadu.h"
#include "protocol.h"
#include "status.h"
#include "usergroup.h"
#include "userlist.h"

typedef uin_t UinType;

class QTimer;
class GaduSocketNotifiers;

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
			konstruuje obiekt UinsList, inicjuj±c go uinem
		**/
		UinsList(UinType uin);

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
//	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		GaduStatus &operator = (const UserStatus &copyMe);

		virtual QPixmap pixmap(eUserStatus status, bool has_desc, bool mobile) const;
		virtual QString pixmapName(eUserStatus status, bool has_desc, bool mobile) const;

		int toStatusNumber() const;
		static int toStatusNumber(eUserStatus status, bool has_desc);

		void fromStatusNumber(int statusNumber, const QString &description);

		virtual UserStatus *copy() const;
		virtual QString protocolName() const;
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
	bool IgnoreResults;

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


enum GaduError
{
	ConnectionServerNotFound,
	ConnectionCannotConnect,
	ConnectionNeedEmail,
	ConnectionInvalidData,
	ConnectionCannotRead,
	ConnectionCannotWrite,
	ConnectionIncorrectPassword,
	ConnectionTlsError,
	ConnectionIntruderError,
	ConnectionUnavailableError,
	ConnectionUnknow,
	ConnectionTimeout,
	Disconnected
};


// ------------------------------------
//            GaduProtocol
// ------------------------------------

/**
	Klasa do obs³ugi protoko³u Gadu-Gadu
**/
class GaduProtocol : public Protocol
{
	Q_OBJECT

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

			Kilka operacji w protokole Gadu-Gadu wymaga od klienta wys³ania do serwera, oprócz
			standardowych pakietów, tak zwanego 'tokena'. Najpierw pobierany jest z serwera identyfikator
			tokena oraz obrazek. Nastêpnie z obrazka odczytywana jest warto¶c tokena (w za³o¿eniach
			mia³o to zapobiegaæ rejestrowaniu nowych u¿ytkowników przez automaty.

			Sloty register, unregister, remindPassword i changePassword inicjuj± pobieranie tokena
			i ustalaj± warto¶æ pola Mode. Pobieranie obrazka realizowane jest przez klasê
			TokenSocketNotifiers. Po pobraniu wywo³ywany jest slot gotToken, który na podstawie warto¶ci
			pola Mode wywo³uje jedn± z funkcji doRegister, doUnregister, doRemindPassword i doChangePassword.
		**/
		enum
		{
			/** Rejestrowanie nowego u¿ytkownika **/
			Register,
			/** Wyrejestrowywanie istniej±cego u¿ytkownika **/
			Unregister,
			/** Przypominanie has³a **/
			RemindPassword,
			/** Zmienianie has³a **/
			ChangePassword
		} Mode;

		/** Identyfikator u¿ytkownika **/
		UinType DataUin;
		/** e-mail u¿ytkownika **/
		QString DataEmail;
		/** stare has³o u¿ytkownika **/
		QString DataPassword;
		/** nowe has³o u¿ytkownika **/
		QString DataNewPassword;
		/** identyfikator tokena **/
		QString TokenId;
		/** warto¶æ tokena **/
		QString TokenValue;

		/** Serwery, z którymi ³aczy siê obiekt. **/
		static QValueList<QHostAddress> ConfigServers;
		/** Numer serwera, do którego obiekt ostatnio próbowa³ siê pod³±czyæ. **/
		unsigned int ServerNr;
		/** Adres serwera, do którego obiekt jest pod³±czony. **/
		QHostAddress ActiveServer;

		/** IP serwera, do którego uda³o siê pod³±czyæ ostatnim razem **/
		QHostAddress lastServerIP;

		/** port serwera, do którego uda³o siê pod³±czyæ ostatnim razem **/
		int lastServerPort;

		/** czy w procedurze ³±czenia mamy korzystaæ z informacji o IP/portu ostatniego serwera? **/
		bool useLastServer;

		int lastTriedServerPort;

		/** Parametry logowania - wymagane przez bibliotekê libgadu **/
		struct gg_login_params LoginParams;
		/** Sesja po³±czenia - wymagane przez bibliotekê libgadu **/
		gg_session* Sess;

		/** liczba ¿±dañ obrazków wys³anych w ci±gu ostatniej minuty**/
		unsigned int sendImageRequests;

		/** czy jeste¶my w trakcie ³±czenia siê z serwerem **/
		bool whileConnecting;

		QHostAddress DccExternalIP;

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

		QTimer* SendUserListTimer;

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
			Lista u¿ytkowników pobrana z serwera w postaci ³añcucha. Warto¶æ ustalana w slocie
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
		GaduProtocol(const GaduProtocol &) : Protocol(QString::null, QString::null) {}
		GaduProtocol &operator=(const GaduProtocol &){return *this;}

	private slots:

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

			@see connecting
			@see connected
			@see connectedSlot
			@see NextStatus
			@see ConfigServers
			@see ServerNr
		**/
		void login();

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
		void imageRequestReceivedSlot(UinType, uint32_t, uint32_t);

		/**
			Slot wywo³ywany po otrzymaniu wiadomo¶ci od serwera.
		**/
		void messageReceived(int, UserListElements, QCString& msg, time_t, QByteArray &formats);

		/**
			Wykonuje zadania co minutê - pinguje sieæ i zeruje licznik
			odebranych obrazków (je¶li jeste¶my po³±czeni).
		**/
		void everyMinuteActions();

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

		void currentStatusChanged(const UserStatus &status, const UserStatus &oldStatus);

	public:
		static void initModule();
		static void closeModule();

		GaduProtocol(const QString &id, QObject *parent = NULL, const char *name = NULL);
		virtual ~GaduProtocol();

		void changeID(const QString &id);
		/**
			Zwraca serwer z którym jeste¶my po³±czeni lub do którego siê w³a¶nie ³±czymy.
			isNull() = hub.
		**/
		QHostAddress activeServer();

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

			grupy maj± postaæ:
			<code>
				grupa_1;grupa_2;grupa_3
			</code>

			@param userList lista u¿ytkowników, która zostanie skonwertowana
			@return ³añcuch reprezentuj±cy listê u¿ytkowników
			@see stringToUserList
			@see streamToUserList
		**/
		QString userListToString(const UserList &userList) const;

		/**
			Konwertujê ³añcuch do listy u¿ytkowników.

			Format ³añcucha jest anologiczny do tego z funkcji userListToString.

			@param source ³añuch, bêd±cy reprezentacj± listy u¿ytkowników
			@see userListToString
			@see streamToUserList
		**/
		QValueList<UserListElement> stringToUserList(const QString &source) const;

		/**
			Odczytuje ze strumienia ³añcuch reprezentuj±cy listê u¿ytkowników i konwertuje
			go go postaci obiektu UserList.

			Format ³añcucha jest anologiczny do tego z funkcji userListToString.

			@param source strumieñ, z którego odczytane zostan± dane
			@see userListToString
			@see stringToUserList
		**/
		QValueList<UserListElement> streamToUserList(QTextStream &source) const;

		/**
			Po jedno sekundowym opó¼nieniu wykonuje próbê po³±czenia.
		**/
		void connectAfterOneSecond();

		/**
			Ustawia adres IP który ma byæ przekazany do serwera GG
			jako adres komputera, który przekierowuje do nas port DCC.
			Zmiana jest uwzglêdniana dopiero przy ponownym po³±czeniu
			z serwerem.
		**/
		void setDccExternalIP(const QHostAddress& ip);

		virtual UserStatus *newStatus() const;
	public slots:
		/**
			Wysy³a wiadomo¶æ bez formatowania tekstu. Je¶li adresatów jest wiêcej ni¿ jeden, to wysy³ana
			jest wiadomo¶æ konferencyjna. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.

			@param users lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param msg wiadomo¶æ, któr± wysy³amy - musi byæ podana w postaci cp1250
			@todo zmieniæ na sendMessage(const UinsList &, QString &) z wewnêtrzn± konwersj± na cp1250
		**/
		int sendMessage(UserListElements users, const char *msg);
		/**
			Wysy³a wiadomo¶æ bez formatowania tekstu. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.

			@param user u¿ytkownikó, do którygo wysy³amy wiadomo¶æ
			@param msg wiadomo¶æ, któr± wysy³amy - musi byæ podana w postaci cp1250
			@todo zmieniæ na sendMessage(const UinsList &, QString &) z wewnêtrzn± konwersj± na cp1250
		**/
		int sendMessage(UserListElement user, const char *msg);

		/**
			Wysy³a wiadomo¶æ z formatowaniem tekstu. Je¶li adresatów jest wiêcej ni¿ jeden, to wysy³ana
			jest wiadomo¶æ konferencyjna. Zwracany jest numer sekwencyjny wiadomo¶ci, je¶li
			przypadkiem by¶my chcieli ¶ledziæ jej potwierdzenie.

			@param users lista u¿ytkowników, do których wysy³amy wiadomo¶æ
			@param msg wiadomo¶æ, któr± wysy³amy - musi byæ podana w postaci cp1250
			@param myLastFormats formatowanie tekstu
			@param myLastFormatsLength ilo¶c znaczników formatuj±cych

			@todo zmieniæ na sendMessageRichText(const UinsList &, QString &, ...)
				z wewnêtrzn± konwersj± na cp1250 oraz z jakim¶ lepszym sposobem formatowania tekstu
		**/
		int sendMessageRichText(UserListElements users, const char *msg, unsigned char *myLastFormats,
		 	unsigned int myLastFormatsLength);

		/**
			Wysy³a pro¶bê o przys³anie obrazka z danymi parametrami.

			@param user u¿ytkownik, od którego chcemy obrazek
			@param size rozmiar obrazka w bajtach
			@param crc32 crc32 pliku
			@todo powinno byæ sendImageRequest(uniqId uint32_t) - info o obrazku zapisywaæ gdzie¶ w ¶rodku
		**/
		bool sendImageRequest(UserListElement user, int size, uint32_t crc32);

		/**
			Wywy³a obrazek o podanych parametrach.

			@param user u¿ytkownik, któremu wysy³amy obrazek
			@param file_name nazwa pliku obrazka
			@param size rozmiar obrazka w bajtach
			@param data zawarto¶æ pliku
			@todo usun±æ parametry size i data - mo¿emy to chyba sami wyznaczyæ
		**/
		bool sendImage(UserListElement user, const QString &file_name, uint32_t size, const char *data);

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
			@todo parametr uin naprawdê potrzebny?
		**/
		void unregisterAccount(UinType uin, const QString &password);

		/**
			Wysy³a has³o na email. Odpowied¼ przychodzi poprzez sygna³ reminded. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param uin nasz uin
			@param email nasz email (musi byæ taki sam jak podczas rejestracji)
			@todo parametr uin naprawdê potrzebny?
		**/
		void remindPassword(UinType uin, const QString& mail);

		/**
			Zmienia nasze has³o. Odpowied¼ przychodzi poprzez sygna³ passwordChanged. Mo¿e
			zostaæ tak¿e wywo³any sygna³ needTokenValue.

			@param uin nasz uin
			@param mail nasz email, jaki podali¶my przy rejestracji
			@param password stare has³o
			@param newPassword nowe has³o
			@todo parametr uin naprawdê potrzebny?
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

		void sendUserListLater();

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
			Zatrzymuje wyszukiwanie.

			@param searchRecord dane u¿yte w wyszukiwaniu
			@see searchInPubdir
		**/
		void stopSearchInPubdir(SearchRecord& searchRecord);

		/**
			Pobiera informacje o danych osobowych z katalogu publicznego.

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
		void dccSocketCreate(UinType uin, uint16_t port, struct gg_dcc **out);
		void dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		void dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		void dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		int dccVoiceSend(struct gg_dcc* d, char* buf, int length);
		void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

	protected slots:
		/* sloty pod³±czane do sygna³ów z klasy UserList */
		void protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);

		void removingUser(UserListElement elem, bool massively, bool last);

		void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);

		void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

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
			wyst±pi³ b³±d po³±czenia
			@param err przyczyna
			@see connectionError
		**/
		void error(GaduError err);

		/**
			dostali¶my pro¶bê o przys³anie obrazka
			@param sender od kogo
			@param size rozmiar pliku
			@param crc32 jego suma kontrolna obliczana przy pomocy crc32
		**/
		void imageRequestReceived(UinType sender, uint32_t size, uint32_t crc32);

		/**
			Otrzymano dane obrazka i zapisano go do pliku.
			@param sender od kogo
			@param size rozmiar pliku
			@param crc32 jego suma kontrolna obliczana przy pomocy crc32
			@param path ¶cie¿ka do zapisanego pliku
		**/
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);

		/**
			dostali¶my od serwera informacjê o zmianie statusu dla kontaktu,
			którego nie mamy na li¶cie
		**/
		void userStatusChangeIgnored(UinType uin);

		/**
			otrzymana wiadomo¶æ systemow±
			@param message tre¶æ wiadomo¶ci wraz z dat±
		**/
		void systemMessageReceived(const QString &message);

		/**
			Otrzymano wiadomo¶æ CTCP.
			Kto¶ nas prosi o po³±czenie dcc, poniewa¿
			jeste¶my za NAT-em
			@param user wywo³uj±cy
			@todo zmieniæ nazwê
		**/
		void dccConnectionReceived(const UserListElement &user);

		/**
			otrzymano nowe wyniki wyszukiwania w katalogu publicznym
			@param searchResults wyniki
			@param seq numer sekwencyjny
			@param lastUin ?
		**/
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);

		/**
			operacja rejestracji nowego konta zosta³a zakoñczona
			@param ok powodzenie operacji
			@param uin nowy numer
			@see doRegisterAccount
		**/
		void registered(bool ok, UinType uin);

		/**
			operacja wyrejestrowania konta zosta³a zakoñczona
			@param ok powodzenie operacji
			@see doUnregisterAccount
		**/
		void unregistered(bool ok);

		/**
			operacja przypomnienia has³a zosta³a zakoñczona
			@param ok powodzenie operacji
			@see doRemindPassword
		**/
		void reminded(bool ok);

		/**
			operacja zmiany has³a zosta³a zakoñczona
			@param ok powodzenie operacji
			@see doChangePassword
		**/
		void passwordChanged(bool ok);

		/**
			operacja eksportu listy kontaktów na serwer zosta³a zakoñczona
			@param ok powodzenie operacji
			@see doExportUserList
		**/
		void userListExported(bool ok);

		/**
			operacja usuniêcia listy kontaktów z serwera zosta³a zakoñczona
			@param ok powodzenie operacji
			@see doClearUserList
		**/
		void userListCleared(bool ok);

		/**
			operacja importu listy kontaktów z serwera zosta³a zakoñczona
			@param ok powodzenie operacji
			@param list je¿eli operacja siê powiod³a, to zaimportowana lista
			@see doImportUserList
		**/
		void userListImported(bool ok, QValueList<UserListElement> list);

		/**
			Sygna³ daje mozliwo¶æ operowania na wiadomo¶ci
			która przysz³a z serwera jeszcze w jej oryginalnej
			formie przed konwersj± na unicode i innymi zabiegami.
			Tre¶æ wiadomo¶ci mo¿na zmieniæ grzebi±c w buforze msg,
			ale uwaga: mo¿na zepsuæ formatowanie tekstu zapisane
			w formats. Oczywi¶cie je równie¿ mo¿na zmieniaæ wed³ug
			opisu protoko³u GG ;)
			Mo¿na te¿ przerwaæ dalsz± obróbkê wiadomo¶ci ustawiaj±c
			stop na true.
		**/
		void messageFiltering(Protocol *protocol, UserListElements senders,
								QCString &msg, QByteArray &formats, bool &stop);
		/**
			Otrzymano wiadomo¶æ któr± trzeba pokazaæ (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre¶æ zdeszyfrowana i zdekodowana do unicode.
			Je¶li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to ¿adna czynno¶æ zwi±zana z obs³ug± tego
			zdarzenia nie zostanie podjêta (tj. wy¶wietlanie wiadomo¶ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo¶ci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool &grab);
		void chatMsgReceived1(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool &grab);
		void chatMsgReceived2(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool grabbed);

		/**
			Wywo³ywane, gdy chcemy odczytaæ token z obrazka
		**/
		void needTokenValue(QPixmap in, QString &out);

};

extern GaduProtocol* gadu;

#endif
