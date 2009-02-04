/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_H
#define GADU_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "protocols/protocol.h"
#include "protocols/status.h"

#include "gadu_exports.h"
#include "userlist.h"

typedef uin_t UinType;

class QTimer;

class AccountData;
class GaduAccountData;
class GaduContactAccountData;
class GaduProtocolSocketNotifiers;

struct SearchRecord;
struct SearchResult;
typedef class QList<SearchResult> SearchResults;

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
	Klasa do obs�ugi protoko�u Gadu-Gadu
**/
class GADUAPI GaduProtocol : public Protocol
{
	Q_OBJECT

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

		Kilka operacji w protokole Gadu-Gadu wymaga od klienta wys�ania do serwera, opr�cz
		standardowych pakiet�w, tak zwanego 'tokena'. Najpierw pobierany jest z serwera identyfikator
		tokena oraz obrazek. Nast�pnie z obrazka odczytywana jest warto�c tokena (w za�o�eniach
		mia�o to zapobiega� rejestrowaniu nowych u�ytkownik�w przez automaty.

		Sloty register, unregister, remindPassword i changePassword inicjuj� pobieranie tokena
		i ustalaj� warto�� pola Mode. Pobieranie obrazka realizowane jest przez klas�
		TokenSocketNotifiers. Po pobraniu wywo�ywany jest slot gotToken, kt�ry na podstawie warto�ci
		pola Mode wywo�uje jedn� z funkcji doRegister, doUnregister, doRemindPassword i doChangePassword.
	**/
	enum
	{
		/** Rejestrowanie nowego u�ytkownika **/
		Register,
		/** Wyrejestrowywanie istniej�cego u�ytkownika **/
		Unregister,
		/** Przypominanie has�a **/
		RemindPassword,
		/** Zmienianie has�a **/
		ChangePassword
	} Mode;

	/** Identyfikator u�ytkownika **/
	UinType DataUin;
	/** e-mail u�ytkownika **/
	QString DataEmail;
	/** stare has�o u�ytkownika **/
	QString DataPassword;
	/** nowe has�o u�ytkownika **/
	QString DataNewPassword;
	/** identyfikator tokena **/
	QString TokenId;
	/** warto�� tokena **/
	QString TokenValue;

	/** Serwery, z kt�rymi �aczy si� obiekt. **/
	static QList<QHostAddress> ConfigServers;
	/** Numer serwera, do kt�rego obiekt ostatnio pr�bowa� si� pod��czy�. **/
	unsigned int ServerNr;
	/** Adres serwera, do kt�rego obiekt jest pod��czony. **/
	QHostAddress ActiveServer;

	/** IP serwera, do kt�rego uda�o si� pod��czy� ostatnim razem **/
	QHostAddress lastServerIP;

	/** port serwera, do kt�rego uda�o si� pod��czy� ostatnim razem **/
	int lastServerPort;

	/** czy w procedurze ��czenia mamy korzysta� z informacji o IP/portu ostatniego serwera? **/
	bool useLastServer;

	int lastTriedServerPort;

	/** Parametry logowania - wymagane przez bibliotek� libgadu **/
	struct gg_login_params LoginParams;

	/** Sesja po��czenia - wymagane przez bibliotek� libgadu **/
	gg_session *Sess;

	/** liczba ��da� obrazk�w wys�anych w ci�gu ostatniej minuty**/
	unsigned int sendImageRequests;

	/** numer sekwencyjny ostatnio wys�anej wiadomo�ci **/
	int seqNumber;

	/** czy jeste�my w trakcie ��czenia si� z serwerem **/
	bool whileConnecting;

	QHostAddress DccExternalIP;

	/**
		Klasa gniazdek ��cz�ca si� z serwerem. Wysy�a sygna�y po wyst�pieniu zdarzenia protoko�u
		(po��czenie, zerwanie po��czenia, nowa wiadomo��).
			@see GaduSocketNotifiers
	**/
	GaduProtocolSocketNotifiers *SocketNotifiers;

	/**
		Zegar pinguj�cy serwer.
	**/
	QTimer *PingTimer;

	QTimer *SendUserListTimer;

	/**
		Zmienna ustawiana w zale�no�ci od tego, czy wysy�amy list� kontakt�w na serwer
		czy te� usuwamy j� z tego serwera. Zako�czenie obydwu tych czynno�ci wywo�uje
		sygna� podpi�ty do slotu userListReplyReceived, kt�ry w zale�no�ci od warto�ci
		tego pola wywo�uje userListCleared albo userListExported.

		@see userListReplyReceived
		@see userListCleared
		@see userListExported
	**/
	bool UserListClear;

	/**
		Lista u�ytkownik�w pobrana z serwera w postaci �a�cucha. Warto�� ustalana w slocie
		userListReplyReceived.

		@see userListReplyReceived
	**/
	QString ImportReply;

	/**
		Ustawianie parametr�w po��czenia proxy. Metoda wywo�ywana podczas logowania.

		@see login
	**/
	void setupProxy();

	/**
		Za pomoc� klasy TokenSocketNotifiers metoda pobiera z serwera GaduGadu token wraz
		z identyfikatorem. Pobrany token jest obs�ugiwany za pomoc� slota gotToken,
		kt�ry pobiera warto�� tokena emituj�c sygna� needTokenValue i nast�pnie wywo�uj�c
		jedn� z metod  doRegisterAccount, doUnregisterAccount(), doRemindPassword(),
		doChangePassword() na podstawie warto�ci pola Mode.

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
		Rejestruje nowe konto. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
		registerAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu registerDone,
		kt�ry emituje sygna� registered.

		@see registerAccount
		@see registered
		@see registerDone
		@see gotToken
	**/
	void doRegisterAccount();

	/**
		Wyrejestrowuje konto. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
		unregisterAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu unregisterDone,
		kt�ry emituje sygna� unregistered.

		@see unregisterAccount
		@see unregistered
		@see unregisterDone
		@see gotToken
	**/
	void doUnregisterAccount();

	/**
		Przypomina has�o. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
		remindPassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu remindDone,
		kt�ry emituje sygna� reminded.

		@see remindPassword
		@see reminded
		@see remindDone
		@see gotToken
	**/
	void doRemindPassword();

	/**
		Zmienia has�o. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
		changePassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu changePasswordDone,
		kt�ry emituje sygna� passwordChanged.

		@see changePassword
		@see passwordChanged
		@see changePasswordDone
		@see gotToken
	**/
	void doChangePassword();

	GaduProtocol(const GaduProtocol &) : Protocol(0, 0) {}
	GaduProtocol & operator = (const GaduProtocol &) {}

	GaduAccountData * gaduAccountData() const;

	UinType uin(Contact contact) const;
	GaduContactAccountData * gaduContactAccountData(Contact contact) const;

	Status::StatusType statusTypeFromIndex(unsigned int index) const;

private slots:
	/**
		Loguje si� do serwera Gadu-Gadu. Po uruchomieniu emituje sygna� connecting. Parametry
		logowania odczytuje z konfiguracji, status logowania pobiera z pola NextStatus.
		��cz�c si�, wybiera kolejne serwery (w przypadku nieudanego po��czenia) wykorzystuj�c
		pola ConfigServers i i ServerNr.

		Po poprawnym zalogowaniu wywo�ywany jest slot connectedSlot, kt�ry emituje sygna�
		GaduProtocol::connected

		Metod� mo�na uruchomi� po�rednio poprzez wywo�anie typu gadu->status().setOnline(),
		kt�re wywo�a slot iWantToGoOnline, kt�ry z kolei (gdy stwierdzi, �e nie jeste�my zalogowani)
		wywo�a procedur�.

		@see connecting
		@see connected
		@see connectedSlot
		@see NextStatus
		@see ConfigServers
		@see ServerNr
	**/
	void login();

	/**
		Wywo�ywany po zarejestrowaniu konta. Emituje registered/

		@see registerAccount
		@see doRegisterAccount
		@see registered
	**/
	void registerDone(bool ok, struct gg_http *);

	/**
		Wywo�ywany po wyrejestrowaniu konta. Emituje unregistered.

		@see unregisterAccount
		@see doUnregisterAccount
		@see unregistered
	**/
	void unregisterDone(bool ok, struct gg_http *);

	/**
		Wywo�ywany po przypomnieniu has�a. Emituje reminded.

		@see remindPassword
		@see doRemindPassword
		@see reminded
	**/
	void remindDone(bool ok, struct gg_http *);

	/**
		Wywo�ywany po zmianie has�a. Emituje passwordChanged.

		@see changePassword
		@see doChangePassword
		@see passwordChanged
	**/
	void changePasswordDone(bool ok, struct gg_http *);

	/**
		Slot wywo�ywany, gdy pobieranie tokena si� nie uda�o.

		@see getToken
	**/
	void tokenError();

	/**
		Slot wywo�ywany, gdy pobieranie tokena si� powiod�o. Emituje needTokenValue

		@see getToken
		@see needTokenValue
	**/
	void gotToken(QString, QPixmap);

	/**
		Slot wywo�ywany po po��czeniu z serwerem. Emituje connected i w��cza pingowanie
		serwera.

		@see connected
	**/
	void connectedSlot();

	/**
		Slot wywo�ywany po roz��czeniu z serwerem. Emituje disconnected i wy��cza pingowanie
		serwera.

		@see disconnected
	**/
	void disconnectedSlot();

	/**
		Slot wywo�ywany po zerwaniu po��czenia przez serwer. Wywo�uje disconnectedSlot(),
		oraz dodatkowo zmienia NextStatus.
	**/
	void socketDisconnectedSlot();


	/**
		Slot wywo�ywany po przekroczeniu czasu po��czenia. Pr�buje po�aczy� ponownie.
	**/
	void connectionTimeoutTimerSlot();

	/**
		Slot wywo�ywane po wyst�pieniu b��du po��czenia. Emituje disconnected i error.

		@see error
		@see disconnected
	**/
	void errorSlot(GaduError);

	/**
		Slot wywo�ywany po otrzymaniu obrazka od serwera. Emituje imageReceivedAndSaved

		@see imageReceivedAndSaved
	**/
	void imageReceived(UinType sender, uint32_t size, uint32_t crc32, const QString &filename, const char *data);

	/**
		Slot wywo�ywany po otrzymaniu pro�by o obrazek od serwera. Wysy�a obrazek.
	**/
	void imageRequestReceivedSlot(UinType, uint32_t, uint32_t);

	/**
		Slot wywo�ywany po otrzymaniu wiadomo�ci od serwera.
	**/
	void messageReceivedSlot(int, ContactList, QString &msg, time_t, QByteArray &formats);

	/**
		Wykonuje zadania co minut� - pinguje sie� i zeruje licznik
		odebranych obrazk�w (je�li jeste�my po��czeni).
	**/
	void everyMinuteActions();

	/**
		Obs�uguje otrzymanie nowych wynik�w wyszukiwania z serwera. Emituje newSearchResults.

		@see newSearchResults
	**/
	void newResults(gg_pubdir50_t res);

	/**
		Nowa wiadomo�� od serwera. Emituje systemMessageReceived

		@see systemMessageReceived
	**/
	void systemMessageReceived(QString &, QDateTime &, int, void *);

	/**
		Pobrano list� u�ytkownik�w z serwera. Emituje userStatusChanged dla ka�dego
		otrzymanego kontaktu oraz userListChanged przed zako�czeniem dzia�ania.

		@see userStatusChanged
		@see userListChanged
	**/
	void userListReceived(const struct gg_event *);

	/**
		Odpowied� od serwera na temat operacji na li�cie u�ytkownik�w. Emituje, w zale�no�ci
		od trybu dzia�ania: userListCleared, userListExported, userListImported.

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
		Przysz�a informacja o dostarczeniu (lub nie) wiadomo�ci.
		Na podstawie statusu emituje odpowiednie sygna�y message*
	**/
	void ackReceived(int seq, uin_t uin, int status);

	void currentStatusChanged(const UserStatus &status, const UserStatus &oldStatus);

protected:
	virtual void changeStatus(Status status);

public:
	static void initModule();

	static int statusToType(Status status);
	static Status typeToStatus(int type);

	GaduProtocol(Account *account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual void setAccount(Account *account);

	unsigned int maxDescriptionLength();

	gg_session * session() { return Sess; }

	void changeID(const QString &id);
	/**
		Zwraca serwer z kt�rym jeste�my po��czeni lub do kt�rego si� w�a�nie ��czymy.
		isNull() = hub.
	**/
	QHostAddress activeServer();

	/**
		Konwertuje list� u�ytkownik�w do postaci �a�cucha.

		�a�cuch wynikowy ma posta�:
		<code>
			opis_u�ytkownika<CR><LF>
			opis_u�ytkownika<CR><LF>
			...
			opis_u�ytkownika<CR><LF>
		</code>

		opis_u�ytkownika ma posta�:
		<code>
			firstName;lastName;nickName;altNick;mobile;grupy;uin;email;0;;0;
		</code>

		grupy maj� posta�:
		<code>
			grupa_1;grupa_2;grupa_3
		</code>

		@param userList lista u�ytkownik�w, kt�ra zostanie skonwertowana
		@return �a�cuch reprezentuj�cy list� u�ytkownik�w
		@see stringToUserList
		@see streamToUserList
	**/
	QString userListToString(const UserList &userList) const;

	/**
		Konwertuj� �a�cuch do listy u�ytkownik�w.

		Format �a�cucha jest anologiczny do tego z funkcji userListToString.

		@param source �a�uch, b�d�cy reprezentacj� listy u�ytkownik�w
		@see userListToString
		@see streamToUserList
	**/
	QList<UserListElement> stringToUserList(const QString &source) const;

	/**
		Odczytuje ze strumienia �a�cuch reprezentuj�cy list� u�ytkownik�w i konwertuje
		go go postaci obiektu UserList.

		Format �a�cucha jest anologiczny do tego z funkcji userListToString.

		@param source strumie�, z kt�rego odczytane zostan� dane
		@see userListToString
		@see stringToUserList
	**/
	QList<UserListElement> streamToUserList(QTextStream &source) const;

	/**
		Po jedno sekundowym op��nieniu wykonuje pr�b� po��czenia.
	**/
	void connectAfterOneSecond();

	/**
		Ustawia adres IP kt�ry ma by� przekazany do serwera GG
		jako adres komputera, kt�ry przekierowuje do nas port DCC.
		Zmiana jest uwzgl�dniana dopiero przy ponownym po��czeniu
		z serwerem.
	**/
	void setDccExternalIP(const QHostAddress& ip);
	int seqNum() { return seqNumber; }
	bool validateUserID(QString &uid);

	virtual QPixmap statusPixmap(Status status);

	// --------------------
	//  DCC
	// --------------------

	void dccRequest(UinType);
	void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

public slots:
	/**
		Wysy�a wiadomo�� bez formatowania tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
		jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
		przypadkiem by�my chcieli �ledzi� jej potwierdzenie.
		@param users lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
		@param mesg wiadomo��, kt�r� wysy�amy - kodowanie zmieniane wewn�trz
	**/
	virtual bool sendMessage(ContactList users, Message &message);

	/**
		Wysy�a pro�b� o przys�anie obrazka z danymi parametrami.

		@param user u�ytkownik, od kt�rego chcemy obrazek
		@param size rozmiar obrazka w bajtach
		@param crc32 crc32 pliku
		@todo powinno by� sendImageRequest(uniqId uint32_t) - info o obrazku zapisywa� gdzie� w �rodku
	**/
	bool sendImageRequest(Contact contact, int size, uint32_t crc32);

	/**
		Wywy�a obrazek o podanych parametrach.

		@param user u�ytkownik, kt�remu wysy�amy obrazek
		@param file_name nazwa pliku obrazka
		@param size rozmiar obrazka w bajtach
		@param data zawarto�� pliku
		@todo usun�� parametry size i data - mo�emy to chyba sami wyznaczy�
	**/
	bool sendImage(Contact contact, const QString &file_name, uint32_t size, const char *data);

	/**
		Rejetrujemy nowe konto. Odpowied� przychodzi poprzez sygna� registered. Mo�e
		zosta� tak�e wywo�any sygna� needTokenValue.

		@param mail nasz email, pole nieobowi�zkowe
		@param password nasze has�o, pole obowi�zkowe
		@see registered
		@see needTokenValue
		@see unregisterAccount
	**/
	void registerAccount(const QString &mail, const QString &password);

	/**
		Wyrejestrowujemy stare konto. Odpowied� przychodzi poprzez sygna� unregistered. Mo�e
		zosta� tak�e wywo�any sygna� needTokenValue.

		@param uin nasz uin
		@param password nasze has�o
		@todo parametr uin naprawd� potrzebny?
	**/
	void unregisterAccount(UinType uin, const QString &password);

	/**
		Wysy�a has�o na email. Odpowied� przychodzi poprzez sygna� reminded. Mo�e
		zosta� tak�e wywo�any sygna� needTokenValue.

		@param uin nasz uin
		@param email nasz email (musi by� taki sam jak podczas rejestracji)
		@todo parametr uin naprawd� potrzebny?
	**/
	void remindPassword(UinType uin, const QString &mail);

	/**
		Zmienia nasze has�o. Odpowied� przychodzi poprzez sygna� passwordChanged. Mo�e
		zosta� tak�e wywo�any sygna� needTokenValue.

		@param uin nasz uin
		@param mail nasz email, jaki podali�my przy rejestracji
		@param password stare has�o
		@param newPassword nowe has�o
		@todo parametr uin naprawd� potrzebny?
	**/
	void changePassword(UinType uin, const QString &mail, const QString &password, const QString &newPassword);

	/**
		Wysy�a list� u�ytkownik�w na serwer. Odpowied� przychodzi przez sygna� userListExported.

		@return false, je�li operacja si� nie powiod�a
		@param userList lista do wys�ania
		@see userListExported
		@todo usun�� warto�� zwracan�
	**/
	bool doExportUserList(const UserList &userList);

	/**
		Usuwa list� u�ytkownik�w z serwera. Odpowied� przychodzi przez sygna� userListCleared.

		@return false, je�li operacja si� nie powiod�a
		@see userListCleared
		@todo usun�� warto�� zwracan�
	**/
	bool doClearUserList();

	/**
		Importuje list� u�ytkownik�w z serwera. Odpowied� przychodzi przez sygna� userListImported.

		@return false, je�li operacja si� nie powiod�a
		@see userListImported
		@todo usun�� warto�� zwracan�
	**/
	bool doImportUserList();

	/**
		Wysy�a nasz� list� u�ytkownik�w na serwer. Uwaga: nie ma to nic wsp�lnego z importem/eksportem.
	**/
	void sendUserList();

	void sendUserListLater();

	/**
		Szuka ludzi w katalogu publicznym. Wyniki przychodz� za pomoca sygna�u newSearchResults.

		@param searchRecord dane do szukania
		@see newSearchResults
		@see searchNextInPubdir
	**/
	void searchInPubdir(SearchRecord &searchRecord);

	/**
		Szuka ludzi w katalogu publicznym. Wyniki przychodz� za pomoca sygna�u newSearchResults.
		@param searchRecord dane do szukania
		@see newSearchResults
		@see searchInPubdir
	**/
	void searchNextInPubdir(SearchRecord &searchRecord);

	/**
		Zatrzymuje wyszukiwanie.

		@param searchRecord dane u�yte w wyszukiwaniu
		@see searchInPubdir
	**/
	void stopSearchInPubdir(SearchRecord &searchRecord);

	/**
		Pobiera informacje o danych osobowych z katalogu publicznego.

		@todo jak to w og�le dzia�a, bo zapomnia�em??
	**/
	void getPersonalInfo(SearchRecord &searchRecord);

	/**
		Ustawia informacje o danych osobowych w katalogu publicznym.

		@todo jak to w og�le dzia�a, bo zapomnia�em??
	**/
	void setPersonalInfo(SearchRecord &searchRecord, SearchResult &newData);

protected slots:
	/* sloty pod��czane do sygna��w z klasy UserList */
	void protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);
	void userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);
	void userAdded(UserListElement elem, bool massively, bool last);
	void removingUser(UserListElement elem, bool massively, bool last);
	void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);
	void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

signals:
	/**
		wyst�pi� b��d po��czenia
		@param err przyczyna
		@see connectionError
	**/
	void error(GaduError err);

	/**
		dostali�my pro�b� o przys�anie obrazka
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
		@param path �cie�ka do zapisanego pliku
	**/
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);

	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Contact);

	/**
		otrzymana wiadomo�� systemow�
		@param message tre�� wiadomo�ci wraz z dat�
	**/
	void systemMessageReceived(const QString &message);

	/**
		Otrzymano wiadomo�� CTCP.
		Kto� nas prosi o po��czenie dcc, poniewa�
		jeste�my za NAT-em
		@param user wywo�uj�cy
		@todo zmieni� nazw�
	**/
	void dccConnectionReceived(Contact contact);

	/**
		otrzymano nowe wyniki wyszukiwania w katalogu publicznym
		@param searchResults wyniki
		@param seq numer sekwencyjny
		@param lastUin ?
	**/
	void newSearchResults(SearchResults &searchResults, int seq, int lastUin);

	/**
		operacja rejestracji nowego konta zosta�a zako�czona
		@param ok powodzenie operacji
		@param uin nowy numer
		@see doRegisterAccount
	**/
	void registered(bool ok, UinType uin);

	/**
		operacja wyrejestrowania konta zosta�a zako�czona
		@param ok powodzenie operacji
		@see doUnregisterAccount
	**/
	void unregistered(bool ok);

	/**
		operacja przypomnienia has�a zosta�a zako�czona
		@param ok powodzenie operacji
		@see doRemindPassword
	**/
	void reminded(bool ok);

	/**
		operacja zmiany has�a zosta�a zako�czona
		@param ok powodzenie operacji
		@see doChangePassword
	**/
	void passwordChanged(bool ok);

	/**
		operacja eksportu listy kontakt�w na serwer zosta�a zako�czona
		@param ok powodzenie operacji
		@see doExportUserList
	**/
	void userListExported(bool ok);

	/**
		operacja usuni�cia listy kontakt�w z serwera zosta�a zako�czona
		@param ok powodzenie operacji
		@see doClearUserList
	**/
	void userListCleared(bool ok);

	/**
		operacja importu listy kontakt�w z serwera zosta�a zako�czona
		@param ok powodzenie operacji
		@param list je�eli operacja si� powiod�a, to zaimportowana lista
		@see doImportUserList
	**/
	void userListImported(bool ok, QList<UserListElement> list);

	/**
		Sygna� daje mozliwo�� operowania na wiadomo�ci
		kt�ra przysz�a z serwera jeszcze w jej oryginalnej
		formie przed konwersj� na unicode i innymi zabiegami.
		Tre�� wiadomo�ci mo�na zmieni� grzebi�c w buforze msg,
		ale uwaga: mo�na zepsu� formatowanie tekstu zapisane
		w formats. Oczywi�cie je r�wnie� mo�na zmienia� wed�ug
		opisu protoko�u GG ;)
		Mo�na te� przerwa� dalsz� obr�bk� wiadomo�ci ustawiaj�c
		stop na true.
	**/
	void rawGaduReceivedMessageFilter(Account *account, ContactList senders, QString &msg, QByteArray &formats, bool &ignore);

	/**
		Wywo�ywane, gdy chcemy odczyta� token z obrazka
	**/
	void needTokenValue(QPixmap in, QString &out);

	void dcc7New(struct gg_dcc7 *);
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

};

#endif
