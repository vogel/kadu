#ifndef KADU_USERLISTELEMENT_H
#define KADU_USERLISTELEMENT_H

#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>
#include <qvariant.h>

#include "status.h"

class ULEPrivate;
class UserGroup;

typedef long int UserListKey;

/**
	Typ opisuj±cy stan powiadamiania.
	\enum NotifyType
	\brief Typ opisuj±cy stan powiadamiania.
**/
enum NotifyType {
	NotifyGlobal = 0, /*!< powiadomienia zale¿ne od globalnych ustawieñ */
	NotifyOff = 1, /*!< powiadomienia wy³±czone */
	NotifyOwn = 2 /*!< indywidualne ustawienie powiadamiania */
};

/**
	Klasa reprezentuj±ca kontakt (u¿ytkownika na li¶cie kontaktów).
	\class UserListElement
	\brief Klasa reprezentuj±ca kontakt.
	Ze zwglêdu na siln± hermetyzacjê, nale¿y unikaæ dawania dostêpu przez wska¼niki / referencje
	(nawet const) na zewn±trz do wewnêtrznych danych.

	Klasa zosta³a zbudowana w taki sposób, ¿e przekazywanie przez warto¶æ jest
	bardzo tanie (¿adne pola nie s± kopiowane). Jednocze¶nie nie mo¿na wykonaæ
	niezale¿nej (nie powi±zanej ze ¼ród³em) kopii obiektu, gdy¿ mog³oby to
	doprowadziæ do stworzenia kilku kontaktów o tych samych danych (czego nale¿y
	siê wystrzegaæ!).
**/
class UserListElement : public QObject
{
	Q_OBJECT
	public:
		/**
			\fn UserListElement()
		**/
		UserListElement();

		/**
			\fn UserListElement(const UserListElement &copyMe)
			Konstruktor kopiuj±cy.
		**/
		UserListElement(const UserListElement &copyMe);

		/**
			\fn virtual ~UserListElement()
			Zwalnia pamiêæ zwi±zan± z kontaktem, gdy licznik odwo³añ == 1
		**/
		virtual ~UserListElement();

		/**
			\fn void operator = (const UserListElement &copyMe)
			Operator kopiuj±cy. Dane bêd± dzielone.
			\param copyMe obiekt klasy UserListElement który bêdzie skopiowany.
		**/
		void operator = (const UserListElement &copyMe);

		inline bool operator==(const UserListElement &u) const {	return key() == u.key();	}

		inline bool operator!=(const UserListElement &u) const {	return key() != u.key();	}

		inline bool operator<(const UserListElement &u) const  {	return key()  < u.key();	}

		/**
			\fn QString ID(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca identyfikator kontaktu we wskazanym protokole.
		**/
		QString ID(const QString &protocolName) const;

		/**
			\fn const UserStatus &status(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca status kontaktu w protokole protocolName.
		**/
		const UserStatus &status(const QString &protocolName) const;

		/**
			\fn QVariant data(const QString &name) const
			\param name nazwa w³asno¶ci
			Zwraca w³asno¶æ (która nie jest zwi±zana z ¿adnym protoko³em)
		**/
		QVariant data(const QString &name) const;

		/**
			\fn QVariant protocolData(const QString &protocolName, const QString &name) const
			\param protocolName identyfikator protoko³u
			\param name nazwa w³a¶no¶ci
			Zwraca w³asno¶æ name dla protoko³u protocolName.
		**/
		QVariant protocolData(const QString &protocolName, const QString &name) const;

		/**
			\fn QStringList protocolList() const
			Zwraca listê identyfikatorów protoko³ów dla danego kontaktu.
		**/
		QStringList protocolList() const;

		/**
			\fn QStringList protocolDataKeys(const QString &protocolName) const
			Zwraca listê identyfikatorów pól dla protoko³u protocolName.
		**/
		QStringList protocolDataKeys(const QString &protocolName) const;

		/**
			\fn QStringList nonProtocolDataKeys() const
			Zwraca listê identyfikatorów pól nie zwi±zanych z ¿adnym protoko³em.
		**/
		QStringList nonProtocolDataKeys() const;

		/**
			\fn QString firstName() const
			Zwraca imiê kontaktu.
		**/
		QString firstName() const;

		/**
			\fn QString lastName() const
			Zwraca nazwisko kontaktu.
		**/
		QString lastName() const;

		/**
			\fn QString nickName() const
			Zwraca pseudonim kontaktu.
		**/
		QString nickName() const;

		/**
			\fn QString altNick() const
			Zwraca wy¶wietlany pseudonim kontaktu.
		**/
		QString altNick() const;

		/**
			\fn QString mobile() const
			Zwraca numer telefonu kontaktu.
		**/
		QString mobile() const;

		/**
			\fn QString email() const
			Zwraca adres E-Mail kontaktu.
		**/
		QString email() const;

		/**
			\fn QString homePhone() const
			Zwraca numer telefonu domowego kontaktu.
		**/
		QString homePhone() const;

		/**
			\fn QString aliveSound(NotifyType &type) const
			Zwraca sposób powiadamiania d¼wiêkiem o zmianach statusu kontaktu.
		**/
		QString aliveSound(NotifyType &type) const;

		/**
			\fn QString messageSound(NotifyType &type) const
			Zwraca sposób powiadamiania d¼wiêkiem o wiadomo¶ciach od kontaktu.
		**/
		QString messageSound(NotifyType &type) const;

		/**
			\fn bool isAnonymous() const
			Zwraca informacjê, czy kontakt jest anonimowy czy nie.
		**/
		bool isAnonymous() const;

		/**
			\fn bool notify() const
			Zwraca informacjê, czy u¿ytkownik jest powiadamiany o kontakcie czy nie.
		**/
		bool notify() const;

		/**
			\fn bool usesProtocol(const QString &name) const
			\param name identyfikator protoko³u
			Zwraca informacjê o tym czy do kontaktu przypisany jest protokó³ o wskazanym identyfikatorze.
		**/
		bool usesProtocol(const QString &name) const;

		/**
			\fn bool hasIPAddress(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca informacjê czy znany jest adres IP we wskazanym protokole.
		**/
		bool hasIP(const QString &protocolName) const;

		/**
			\fn QHostAddress IP(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca adres IP kontaktu.
		**/
		QHostAddress IP(const QString &protocolName) const;

		/**
			\fn QString DNSName(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca nazwê domeny kontaktu (je¶li znaleziona).
		**/
		QString DNSName(const QString &protocolName) const;

		/**
			\fn short port(const QString &protocolName) const
			\param protocolName identyfikator protoko³u
			Zwraca numer portu kontaktu.
		**/
		short port(const QString &protocolName) const;

	public slots:
		/**
			\fn QVariant setData(const QString &name, const QVariant &val, bool massively = false, bool last = false)
			ustawia w³asno¶æ kontaktu stowarzyszon± z nazw± "name"
			zwraca star± warto¶æ
			\param name nazwa w³asno¶ci
			\param val nowa warto¶æ
			\param massively true, gdy jest to czê¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana

			pola bezprotoko³owe:

				QStringList Groups;
				QString FirstName; Imiê kontaktu.
				QString LastName; Nazwisko kontaktu.
				QString NickName; Pseudonim kontaktu.
				QString AltNick; Pseudonim kontaktu, który jest wy¶wietlany na li¶cie.
				QString Mobile; Numer telefonu kontaktu.
				QString Email; E-Mail kontaktu.
				bool Anonymous; Informuje, czy kontakt jest anonimowy czy nie.
				bool Notify; Informuje czy mamy w³±czone powiadamianie o kontakcie.

				NotifyType AliveSound; Przechowuje informacjê o sposobie powiadamiania
										o zmianie statusu kontaku d¼wiêkiem.
				QString OwnAliveSound; Je¶li sposób powiadamiania o zmianie statusu kontaktu ma warto¶æ OWN,
									to ta zmienna przechowuje nazwê pliku d¼wiêkowego do odtworzenia.

				NotifyType MessageSound; Przechowuje informacjê o sposobie powiadamiania
										o nowej wiadomo¶ci od kontaktu d¼wiêkiem.
				QString OwnMessageSound;  Je¶li sposób powiadamiania o nowej wiadomo¶ci od kontaktu ma warto¶æ OWN,
										to ta zmienna przechowuje nazwê pliku d¼wiêkowego do odtworzenia.

				QString HomePhone; Numer telefonu domowego kontaktu.
		**/
		QVariant setData(const QString &name, const QVariant &val, bool massively = false, bool last = false);

		/**
			\fn QVariant setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko³u
			\param name nazwa w³asno¶ci
			\param val nowa warto¶æ
			\param massively true, gdy jest to czê¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			\return stara warto¶æ
			Ustawia w³asno¶æ name zwi±zan± z protoko³em protocolName na val.

			dla protoko³u "Gadu" dostêpne s± nastêpuj±ce pola:

				int MaxImageSize; Maksymalny rozmiar obrazka, jak± mo¿e przyj±æ kontakt.
				QHostAddress IP; Adres IP kontaktu (je¶li wykryty).
				QString DNSName; Nazwa domenu kontaktu (je¶li znaleziona).
				short Port; Port kontaktu (je¶li wykryty).
				int Version; Wersja protoko³u u¿ywanego przez kontakt.
				bool Blocking; Informuje czy blokujemy kontakt, czy nie.
				bool OfflineTo; Informuje czy mamy w³±czony tryb "niedostêpny dla kontaktu" dla tego kontaktu.
		**/
		QVariant setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively = false, bool last = false);

		/**
			\fn void addProtocol(const QString &protocolName, const QString &id, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko³u
			\param id identyfikator w tym protokole w postaci napisu
			\param massively true, gdy jest to czê¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Dodaje do kontaktu informacjê o u¿ywanym protokole.
		**/
		void addProtocol(const QString &protocolName, const QString &id, bool massively = false, bool last = false);

		/**
			\fn void deleteProtocol(const QString &protocolName, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko³u
			\param massively true, gdy jest to czê¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Usuwa informacje o protokole.
		**/
		void deleteProtocol(const QString &protocolName, bool massively = false, bool last = false);

		/**
			\fn void setStatus(const QString &protocolName, const UserStatus &status, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko³u
			\param status nowy status
			\param massively true, gdy jest to czê¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Zmienia status kontaktu w protokolej protocolName.
		**/
		void setStatus(const QString &protocolName, const UserStatus &status, bool massively = false, bool last = false);

		/**
			\fn void setFirstName(const QString &firstName)
			Ustawia imiê dla kontaktu.
			\param firstName imiê, które zostanie przydzielone kontaktowi.
		**/
		void setFirstName(const QString &firstName);

		/**
			\fn void setLastName(const QString &lastName)
			Ustawia nazwisko dla kontaktu.
			\param lastName nazwisko, które zostanie przydzielone kontaktowi.
		**/
		void setLastName(const QString &lastName);

		/**
			\fn void setNickName(const QString &nickName)
			Ustawia pseudonim dla kontaktu.
			\param nickName pseudonim, który zostanie przydzielony kontaktowi.
		**/
		void setNickName(const QString &nickName);

		/**
			\fn void setAltNick(const QString &altNick)
			Ustawia wy¶wietlany pseudonim dla kontaktu.
			\param altNick wy¶wietlany pseudonim, który zostanie przydzielony kontaktowi.
		**/
		void setAltNick(const QString &altNick);

		/**
			\fn void setMobile(const QString &mobile)
			Ustawia numer telefonu dla kontaktu.
			\param mobile numer telefonu, który zostanie przydzielony kontaktowi.
		**/
		void setMobile(const QString &mobile);

		/**
			\fn void setEmail(const QString &email)
			Ustawia adres E-Mail dla kontaktu.
			\param email adres, który zostanie przydzielony kontaktowi.
		**/
		void setEmail(const QString &email);

		/**
			\fn void setAnonymous(const bool anonymous)
			Ustawia stan anonimowo¶ci kontaktu.
			\param anonymous warto¶æ logiczna informuj±ca, ¿e kontakt jest anonimowy, lub nie.
		**/
		void setAnonymous(const bool &anonymous);

		/**
			\fn void setNotify(const bool notify)
			Ustawia stan powiadamiania o kontakcie.
			\param notify warto¶æ logiczna informuj±ca, czy u¿ytkownik ma byæ powiadamiany o kontakcie.
		**/
		void setNotify(const bool &notify);

		/**
			\fn void setHomePhone(const QString &phone)
			Ustawia numer telefonu domowego dla kontaktu.
			\param phone numer telefonu, który zostanie przydzielony kontaktowi.
		**/
		void setHomePhone(const QString &phone);

		/**
			\fn void setAliveSound(NotifyType type, const QString &file = QString::null)
			Ustawia sposób powiadamiania d¼wiêkiem o zmianie statusu przez kontakt.
			\param type sposób powiadamiania.
			\arg \c GLOBAL powiadomienia zale¿ne od globalnych ustawieñ.
			\arg \c OFF powiadomienia wy³±czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d¼wiêkowy, wymagany dla indywidualnego ustawienia powiadomieñ.
		**/
		void setAliveSound(NotifyType type, const QString &file = QString::null);

		/**
			\fn void setMessageSound(NotifyType type, const QString &file = QString::null)
			Ustawia sposób powiadamiania d¼wiêkiem o nowej wiadomo¶ci od kontaktu.
			\param type sposób powiadamiania.
			\arg \c GLOBAL powiadomienia zale¿ne od globalnych ustawieñ.
			\arg \c OFF powiadomienia wy³±czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d¼wiêkowy, wymagany dla indywidualnego ustawienia powiadomieñ.
		**/
		void setMessageSound(NotifyType type, const QString &file = QString::null);

		/**
			\fn void setAddressAndPort(const QString &protocolName, const QHostAddress &ip, short port)
			\param protocolName identyfikator protoko³u
			\param ip adres IP
			\param port port
			Ustawia adres IP i port dla wskazanego protoko³u.
		**/
		void setAddressAndPort(const QString &protocolName, const QHostAddress &ip, short port);

		/**
			\fn void setDNSName(const QString &protocolName, const QString &dnsname)
			\param protocolName identyfikator protoko³u
			\param dnsname nowa domena
			Ustawia domenê dla wskazanego protoko³u.
		**/
		void setDNSName(const QString &protocolName, const QString &dnsname);

		/**
			\fn void refreshDNSName(const QString &protocolName)
			\param protocolName identyfikator protoko³u
			Wywo³uje zapytanie o nazwê domeny dla kontaktu oraz wype³nia odpowiednie pole kontaktu,
			gdy domena zostanie odnaleziona.
		**/
		void refreshDNSName(const QString &protocolName);

	protected:
		/**
			\fn UserListKey key() const
			Zwraca klucz po którym jest identyfikowany kontakt
		**/
		UserListKey key() const;

	private:
		ULEPrivate *privateData;
		static unsigned long int used;
		friend class UserGroup;
};

#endif
