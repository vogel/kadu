/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_H
#define GADU_PROTOCOL_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "userlist.h"

#include "services/gadu-chat-service.h"
#include "services/gadu-contact-list-service.h"

#include "gadu_exports.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class GaduAccountData;
class GaduContactAccountData;
class GaduProtocolSocketNotifiers;

class GADUAPI GaduProtocol : public Protocol
{
	Q_OBJECT
	Q_DISABLE_COPY(GaduProtocol);

public:
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

private:
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;

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
		Ustawianie parametr�w po��czenia proxy. Metoda wywo�ywana podczas logowania.

		@see login
	**/
	void setupProxy();

	GaduAccountData * gaduAccountData() const;

	UinType uin(Contact contact) const;
	GaduContactAccountData * gaduContactAccountData(Contact contact) const;

	Status::StatusType statusTypeFromIndex(unsigned int index) const;

	GaduProtocolSocketNotifiers * socketNotifiers() { return SocketNotifiers; }

	friend class GaduChatService;
	friend class GaduContactListService;

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
		Wykonuje zadania co minut� - pinguje sie� i zeruje licznik
		odebranych obrazk�w (je�li jeste�my po��czeni).
	**/
	void everyMinuteActions();

	/**
		Pobrano list� u�ytkownik�w z serwera. Emituje userStatusChanged dla ka�dego
		otrzymanego kontaktu oraz userListChanged przed zako�czeniem dzia�ania.

		@see userStatusChanged
		@see userListChanged
	**/
	void userListReceived(const struct gg_event *);

	/**
		Informacja o zmianie statusu kontaktu. Emituje userStatusChanged oraz userListChanged.

		@see userStatusChanged
		@see userListChanged
	**/
	void userStatusChanged(const struct gg_event *);

	void currentStatusChanged(const UserStatus &status, const UserStatus &oldStatus);

protected:
	virtual void changeStatus(Status status);

public:
	static void initModule();

	static int statusToType(Status status);
	static Status typeToStatus(int type);

	GaduProtocol(Account *account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }

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
	bool validateUserID(QString &uid);

	virtual QPixmap statusPixmap(Status status);

	// --------------------
	//  DCC
	// --------------------

	void dccRequest(UinType);
	void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

public slots:

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
		Wysy�a nasz� list� u�ytkownik�w na serwer. Uwaga: nie ma to nic wsp�lnego z importem/eksportem.
	**/
	void sendUserList();

	void sendUserListLater();

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
		Otrzymano wiadomo�� CTCP.
		Kto� nas prosi o po��czenie dcc, poniewa�
		jeste�my za NAT-em
		@param user wywo�uj�cy
		@todo zmieni� nazw�
	**/
	void dccConnectionReceived(Contact contact);

	void dcc7New(struct gg_dcc7 *);
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

};

#endif // GADU_PROTOCOL_H
