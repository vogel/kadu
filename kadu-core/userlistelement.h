/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_USERLISTELEMENT_H
#define KADU_USERLISTELEMENT_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "contacts/contact.h"
#include "exports.h"

class QHostAddress;

class Account;
class ULEPrivate;
class UserGroup;
class UserStatus;

typedef long int UserListKey;

class UserListElement;
ulong qHash(const UserListElement &index);

/**
	Typ opisuj�cy stan powiadamiania.
	\enum NotifyType
	\brief Typ opisuj�cy stan powiadamiania.
**/
enum NotifyType {
	NotifyGlobal = 0, /*!< powiadomienia zale�ne od globalnych ustawie� */
	NotifyOff = 1, /*!< powiadomienia wy��czone */
	NotifyOwn = 2 /*!< indywidualne ustawienie powiadamiania */
};

/**
	Klasa reprezentuj�ca kontakt (u�ytkownika na li�cie kontakt�w).
	\class UserListElement
	\brief Klasa reprezentuj�ca kontakt.
	Ze zwgl�du na siln� hermetyzacj�, nale�y unika� dawania dost�pu przez wska�niki / referencje
	(nawet const) na zewn�trz do wewn�trznych danych.

	Klasa zosta�a zbudowana w taki spos�b, �e przekazywanie przez warto�� jest
	bardzo tanie (�adne pola nie s� kopiowane). Jednocze�nie nie mo�na wykona�
	niezale�nej (nie powi�zanej ze �r�d�em) kopii obiektu, gdy� mog�oby to
	doprowadzi� do stworzenia kilku kontakt�w o tych samych danych (czego nale�y
	si� wystrzega�!).
**/
class KADUAPI UserListElement : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<ULEPrivate> privateData;

	friend ulong qHash(const UserListElement &index);

public:
	/**
		\fn UserListElement()
	**/
	UserListElement();

	/**
		\fn UserListElement(const UserListElement &copyMe)
		Konstruktor kopiuj�cy.
	**/
	UserListElement(const UserListElement &copyMe);

	/**
		\fn virtual ~UserListElement()
		Zwalnia pami�� zwi�zan� z kontaktem, gdy licznik odwo�a� == 1
	**/
	virtual ~UserListElement();

	/**
		\fn void operator = (const UserListElement &copyMe)
		Operator kopiuj�cy. Dane b�d� dzielone.
		\param copyMe obiekt klasy UserListElement kt�ry b�dzie skopiowany.
	**/
	UserListElement & operator = (const UserListElement &copyMe);

	inline bool operator == (const UserListElement &u) const { return privateData == u.privateData; }

	inline bool operator != (const UserListElement &u) const { return privateData != u.privateData; }

	inline bool operator < (const UserListElement &u) const { return privateData.data() < u.privateData.data(); }

	/**
		\fn QString ID(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca identyfikator kontaktu we wskazanym protokole.
	**/
	QString ID(const QString &protocolName) const;

	/**
		\fn const UserStatus &status(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca status kontaktu w protokole protocolName.
	**/
	UserStatus & status(const QString &protocolName) const;

	/**
		\fn QVariant data(const QString &name) const
		\param name nazwa w�asno�ci
		Zwraca w�asno�� (kt�ra nie jest zwi�zana z �adnym protoko�em)
	**/
	QVariant data(const QString &name) const;

	/**
		\fn QVariant protocolData(const QString &protocolName, const QString &name) const
		\param protocolName identyfikator protoko�u
		\param name nazwa w�a�no�ci
		Zwraca w�asno�� name dla protoko�u protocolName.
	**/
	QVariant protocolData(const QString &protocolName, const QString &name) const;

	/**
		\fn QStringList protocolList() const
		Zwraca list� identyfikator�w protoko��w dla danego kontaktu.
	**/
	QStringList protocolList() const;

	/**
		\fn QStringList protocolDataKeys(const QString &protocolName) const
		Zwraca list� identyfikator�w p�l dla protoko�u protocolName.
	**/
	QStringList protocolDataKeys(const QString &protocolName) const;

	/**
		\fn QStringList nonProtocolDataKeys() const
		Zwraca list� identyfikator�w p�l nie zwi�zanych z �adnym protoko�em.
	**/
	QStringList nonProtocolDataKeys() const;

	/**
		\fn QString firstName() const
		Zwraca imi� kontaktu.
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
		Zwraca wy�wietlany pseudonim kontaktu.
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
		Zwraca spos�b powiadamiania d�wi�kiem o zmianach statusu kontaktu.
	**/
	QString aliveSound(NotifyType &type) const;

	/**
		\fn QString messageSound(NotifyType &type) const
		Zwraca spos�b powiadamiania d�wi�kiem o wiadomo�ciach od kontaktu.
	**/
	QString messageSound(NotifyType &type) const;

	/**
		\fn bool isAnonymous() const
		Zwraca informacj�, czy kontakt jest anonimowy czy nie.
	**/
	bool isAnonymous() const;

	/**
		\fn bool notify() const
		Zwraca informacj�, czy u�ytkownik jest powiadamiany o kontakcie czy nie.
	**/
	bool notify() const;

	/**
		\fn bool usesProtocol(const QString &name) const
		\param name identyfikator protoko�u
		Zwraca informacj� o tym czy do kontaktu przypisany jest protok�� o wskazanym identyfikatorze.
	**/
	bool usesProtocol(const QString &name) const;

	/**
		\fn bool hasIPAddress(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca informacj� czy znany jest adres IP we wskazanym protokole.
	**/
	bool hasIP(const QString &protocolName) const;

	/**
		\fn QHostAddress IP(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca adres IP kontaktu.
	**/
	QHostAddress IP(const QString &protocolName) const;

	/**
		\fn QString DNSName(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca nazw� domeny kontaktu (je�li znaleziona).
	**/
	QString DNSName(const QString &protocolName) const;

	/**
		\fn short port(const QString &protocolName) const
		\param protocolName identyfikator protoko�u
		Zwraca numer portu kontaktu.
	**/
	unsigned short port(const QString &protocolName) const;

	bool containsGroup(UserGroup *group) const;
	void addGroup(UserGroup *group) const;
	void removeGroup(UserGroup *group) const;

public slots:
	/**
		\fn QVariant setData(const QString &name, const QVariant &val, bool massively = false, bool last = false)
		ustawia w�asno�� kontaktu stowarzyszon� z nazw� "name"
		zwraca star� warto��
		\param name nazwa w�asno�ci
		\param val nowa warto��
		\param massively true, gdy jest to cz��� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana

		pola bezprotoko�owe:

			QStringList Groups;
			QString FirstName; Imi� kontaktu.
			QString LastName; Nazwisko kontaktu.
			QString NickName; Pseudonim kontaktu.
			QString AltNick; Pseudonim kontaktu, kt�ry jest wy�wietlany na li�cie.
			QString Mobile; Numer telefonu kontaktu.
			QString Email; E-Mail kontaktu.
			bool Anonymous; Informuje, czy kontakt jest anonimowy czy nie.
			bool Notify; Informuje czy mamy w��czone powiadamianie o kontakcie.

			NotifyType AliveSound; Przechowuje informacj� o sposobie powiadamiania
									o zmianie statusu kontaku d�wi�kiem.
			QString OwnAliveSound; Je�li spos�b powiadamiania o zmianie statusu kontaktu ma warto�� OWN,
								to ta zmienna przechowuje nazw� pliku d�wi�kowego do odtworzenia.

			NotifyType MessageSound; Przechowuje informacj� o sposobie powiadamiania
									o nowej wiadomo�ci od kontaktu d�wi�kiem.
			QString OwnMessageSound;  Je�li spos�b powiadamiania o nowej wiadomo�ci od kontaktu ma warto�� OWN,
									to ta zmienna przechowuje nazw� pliku d�wi�kowego do odtworzenia.

			QString HomePhone; Numer telefonu domowego kontaktu.
	**/
	QVariant setData(const QString &name, const QVariant &val, bool massively = false, bool last = false) const;

	/**
		\fn QVariant setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively = false, bool last = false)
		\param protocolName identyfikator protoko�u
		\param name nazwa w�asno�ci
		\param val nowa warto��
		\param massively true, gdy jest to cz��� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		\return stara warto��
		Ustawia w�asno�� name zwi�zan� z protoko�em protocolName na val.

		dla protoko�u "Gadu" dost�pne s� nast�puj�ce pola:

			int MaxImageSize; Maksymalny rozmiar obrazka, jak� mo�e przyj�� kontakt.
			QHostAddress IP; Adres IP kontaktu (je�li wykryty).
			QString DNSName; Nazwa domenu kontaktu (je�li znaleziona).
			short Port; Port kontaktu (je�li wykryty).
			int Version; Wersja protoko�u u�ywanego przez kontakt.
			bool Blocking; Informuje czy blokujemy kontakt, czy nie.
			bool OfflineTo; Informuje czy mamy w��czony tryb "niedost�pny dla kontaktu" dla tego kontaktu.
	**/
	QVariant setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively = false, bool last = false) const;

	/**
		\fn void addProtocol(const QString &protocolName, const QString &id, bool massively = false, bool last = false)
		\param protocolName identyfikator protoko�u
		\param id identyfikator w tym protokole w postaci napisu
		\param massively true, gdy jest to cz��� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		Dodaje do kontaktu informacj� o u�ywanym protokole.
	**/
	void addProtocol(const QString &protocolName, const QString &id, bool massively = false, bool last = false) const;

	/**
		\fn void deleteProtocol(const QString &protocolName, bool massively = false, bool last = false)
		\param protocolName identyfikator protoko�u
		\param massively true, gdy jest to cz��� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		Usuwa informacje o protokole.
	**/
	void deleteProtocol(const QString &protocolName, bool massively = false, bool last = false) const;

	/**
		\fn void setStatus(const QString &protocolName, const UserStatus &status, bool massively = false, bool last = false)
		\param protocolName identyfikator protoko�u
		\param status nowy status
		\param massively true, gdy jest to cz��� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		Zmienia status kontaktu w protokolej protocolName.
	**/
	void setStatus(const QString &protocolName, const UserStatus &status, bool massively = false, bool last = false) const;

	/**
		\fn void setFirstName(const QString &firstName)
		Ustawia imi� dla kontaktu.
		\param firstName imi�, kt�re zostanie przydzielone kontaktowi.
	**/
	void setFirstName(const QString &firstName) const;

	/**
		\fn void setLastName(const QString &lastName)
		Ustawia nazwisko dla kontaktu.
		\param lastName nazwisko, kt�re zostanie przydzielone kontaktowi.
	**/
	void setLastName(const QString &lastName) const;

	/**
		\fn void setNickName(const QString &nickName)
		Ustawia pseudonim dla kontaktu.
		\param nickName pseudonim, kt�ry zostanie przydzielony kontaktowi.
	**/
	void setNickName(const QString &nickName) const;

	/**
		\fn void setAltNick(const QString &altNick)
		Ustawia wy�wietlany pseudonim dla kontaktu.
		\param altNick wy�wietlany pseudonim, kt�ry zostanie przydzielony kontaktowi.
	**/
	void setAltNick(const QString &altNick) const;

	/**
		\fn void setMobile(const QString &mobile)
		Ustawia numer telefonu dla kontaktu.
		\param mobile numer telefonu, kt�ry zostanie przydzielony kontaktowi.
	**/
	void setMobile(const QString &mobile) const;

	/**
		\fn void setEmail(const QString &email)
		Ustawia adres E-Mail dla kontaktu.
		\param email adres, kt�ry zostanie przydzielony kontaktowi.
	**/
	void setEmail(const QString &email) const;

	/**
		\fn void setAnonymous(const bool anonymous)
		Ustawia stan anonimowo�ci kontaktu.
		\param anonymous warto�� logiczna informuj�ca, �e kontakt jest anonimowy, lub nie.
	**/
	void setAnonymous(const bool &anonymous) const;

	/**
		\fn void setNotify(const bool notify)
		Ustawia stan powiadamiania o kontakcie.
		\param notify warto�� logiczna informuj�ca, czy u�ytkownik ma by� powiadamiany o kontakcie.
	**/
	void setNotify(const bool &notify) const;

	/**
		\fn void setHomePhone(const QString &phone)
		Ustawia numer telefonu domowego dla kontaktu.
		\param phone numer telefonu, kt�ry zostanie przydzielony kontaktowi.
	**/
	void setHomePhone(const QString &phone) const;

	/**
		\fn void setAliveSound(NotifyType type, const QString &file = QString::null)
		Ustawia spos�b powiadamiania d�wi�kiem o zmianie statusu przez kontakt.
		\param type spos�b powiadamiania.
		\arg \c GLOBAL powiadomienia zale�ne od globalnych ustawie�.
		\arg \c OFF powiadomienia wy��czone.
		\arg \c OWN indywidualne ustawienie powiadamiania/

		\param file plik d�wi�kowy, wymagany dla indywidualnego ustawienia powiadomie�.
	**/
	void setAliveSound(NotifyType type, const QString &file = QString::null) const;

	/**
		\fn void setMessageSound(NotifyType type, const QString &file = QString::null)
		Ustawia spos�b powiadamiania d�wi�kiem o nowej wiadomo�ci od kontaktu.
		\param type spos�b powiadamiania.
		\arg \c GLOBAL powiadomienia zale�ne od globalnych ustawie�.
		\arg \c OFF powiadomienia wy��czone.
		\arg \c OWN indywidualne ustawienie powiadamiania/

		\param file plik d�wi�kowy, wymagany dla indywidualnego ustawienia powiadomie�.
	**/
	void setMessageSound(NotifyType type, const QString &file = QString::null) const;

	/**
		\fn void setAddressAndPort(const QString &protocolName, const QHostAddress &ip, short port)
		\param protocolName identyfikator protoko�u
		\param ip adres IP
		\param port port
		Ustawia adres IP i port dla wskazanego protoko�u.
	**/
	void setAddressAndPort(const QString &protocolName, const QHostAddress &ip, unsigned short port) const;

	/**
		\fn void setDNSName(const QString &protocolName, const QString &dnsname)
		\param protocolName identyfikator protoko�u
		\param dnsname nowa domena
		Ustawia domen� dla wskazanego protoko�u.
	**/
	void setDNSName(const QString &protocolName, const QString &dnsname) const;

	/**
		\fn void refreshDNSName(const QString &protocolName)
		\param protocolName identyfikator protoko�u
		Wywo�uje zapytanie o nazw� domeny dla kontaktu oraz wype�nia odpowiednie pole kontaktu,
		gdy domena zostanie odnaleziona.
	**/
	void refreshDNSName(const QString &protocolName) const;

	Contact toContact(Account *account);
	static UserListElement fromContact(Contact contact, Account *account);

};

#endif
