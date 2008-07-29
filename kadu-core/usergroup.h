/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_USERGROUP_H
#define KADU_USERGROUP_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QVariant>

#include "userlistelement.h"

#include "exports.h"

class UserListElements;

enum BehaviourForAnonymous {TrueForAnonymous, FalseForAnonymous};

/**
	\class UserGroup
	Klasa odpowiedzialna za przechowywanie grup kontakt�w. Daje mo�liwo��
	bardzo szybkiego wyszukiwania oraz informowania o zdarzeniach zwi�zanych
	z kontaktami (zmiana statusu, dodanie, usuni�cie, zmiana danych).
	Patrz: UserList, GroupsManager, UsersWithDescription, OnlineUsers,
	       BlockedUsers, BlockingUsers, AnonymousUsers
	Je�eli nie potrzebujesz informacji o zdarzeniach, lepiej u�y� jest klasy
	UserListElements.

	Parametr "last" w sygna�ach dost�pnych w tej klasie, powinien by� u�ywany
	tylko gdy mamy absolutn� pewno��, �e zmiana danych nast�pi�a dla wszystkich
	kontakt�w z tej grupy (i ani jednego wi�cej). Dlatego te� bezpieczne jest
	u�ywanie tego parametru tylko w slotach pod��czonych do klasy UserList.
	Je�eli mimo to potrzebujemy skorzysta� z parametru massively w celu wykonania
	jakiej� czynno�ci dopiero po zako�czeniu zmian, mo�emy pos�u�y� si� QTimerem
	z czasem 0 (czyli po oddaniu sterowania p�tli zdarze� Qt) i jednokrotnym
	uruchomieniem. Przyk�ad:
	<code>
	class MojaKlasa
	{
		...
		QTimer refreshTimer;
		...
	};
	MojaKlasa::MojaKlasa()
	{
		...
		connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
		...
	}
	inline void MojaKlasa::refreshLater()
	{
		refreshTimer.start(0, true);
	}

	void MojaKlasa::refresh()
	{
		...//bardzo kosztowne od�wie�anie
	}

	void MojaKlasa::userAdded(UserListElement elem, bool massively, bool last)
	{
		if (massively)
		{
			akcja1; //np jakie� buforowanie
			...
			refreshLater();
		}
		else
		{
			akcja2;
			...
			refresh();
		}
	}
	</code>
**/
class KADUAPI UserGroup : public QObject, public QSet<UserListElement>
{
	Q_OBJECT

	/* standardowy operator kopiowania zepsu�by obiekt �r�d�owy */
	virtual UserGroup & operator = (const UserGroup &) {return *this;}

	/* nie da si� por�wnywa� obiekt�w tej klasy */
	bool operator == (const UserGroup &) const {return false;}

protected:
	friend class UserListElement;
	friend class ULEPrivate;

public:
	/**
		\fn UserGroup(int size, const char *name = 0)
		\param name nazwa grupy
	**/
	UserGroup();

	UserGroup(const UserGroup &copy);

	/**
		\fn UserGroup(const QValueList<UserListElement> &group, const char *name = 0)
		\param group
		\param name nazwa grupy
	**/
	UserGroup(const QList<UserListElement> &group);

	/**
		\fn virtual ~UserGroup()
		Zwalnia pami�� zwi�zan� z grup�
	**/
	virtual ~UserGroup();

	/**
		\fn bool equals(UserListElements users) const
		\param users
		Z�o�ono�� O(users.count()).
	**/
	bool equals(const UserListElements users) const;

	/**
		\fn UserListElement byID(const QString &protocolName, const QString &id)
		\param protocolName identyfikator protoko�u
		\param id identyfikator kontaktu
		Zwraca element listy kontakt�w identyfikowany przez nazw�
		protoko�u i identyfikator w tym�e protokole.
		Je�eli nie znajdzie si� taki kontakt, to tworzony jest nowy
		anonimowy kontakt i dodawany do listy.
		Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana.
	**/
	UserListElement byID(const QString &protocolName, const QString &id);

	/**
		\fn UserListElement byAltNick(const QString &altnick)
		\param altnick wy�wietlany pseudonim wg. kt�rego nast�puje wyszukiwanie.
		\return obiekt reprezentuj�cy kontakt o podanym pseudonimie wy�wietlanym.
		Wyszukuje kontakt po wy�wietlanym pseudonimie.
		Je�eli nie znajdzie si� taki kontakt, to tworzony jest nowy
		anonimowy kontakt i dodawany do listy.
		Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana do O(1).
	**/
	UserListElement byAltNick(const QString &altnick);

	/**
		\fn bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const
		\param protocolName identyfikator protoko�u
		\param id identyfikator kontaktu
		\param beh spos�b zachowania dla anonim�w
		Sprawdza czy grupa zawiera kontakt o danym identyfikatorze we wskazanym protokole.
		W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
			TrueForAnonymous (domy�lnie), to zwraca prawd�
			FalseForAnonymous, to zwraca fa�sz
		Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana.
	**/
	bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const;

	/**
		\fn bool contains(UserListElement elem, BehaviourForAnonymous beh = TrueForAnonymous) const
		\param elem sprawdzany kontakt
		\param beh spos�b zachowania dla anonim�w
		Sprawdza czy grupa zawiera wskazany kontakt.
		W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
			TrueForAnonymous (domy�lnie), to  zwraca prawd�
			FalseForAnonymous, to zwraca fa�sz
		Z�o�ono�� O(1).
	**/
	bool contains(const UserListElement &elem, BehaviourForAnonymous beh = TrueForAnonymous) const;

	/**
		\fn bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const
		\param altnick wy�wietlany pseudonim
		\param beh spos�b zachowania dla anonim�w
		Sprawdza czy grupa zawiera kontakt o danym pseudonimie wy�wietlanym.
		W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
			TrueForAnonymous (domy�lnie), to  zwraca prawd�
			FalseForAnonymous, to zwraca fa�sz
		Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana do O(1).
	**/
	bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const;

	/**
		\fn UserListElements toUserListElements() const
		Zwraca list� kontakt�w w postaci obiektu klasy UserListElements
	**/
	UserListElements toUserListElements() const;

	/**
		\fn QStringList altNicks() const
		Zwraca list� nick�w kontakt�w nale��cych do grupy w postaci
		obiektu klasy QStringList.
	**/
	QStringList altNicks() const;

	/**
		\fn void clear()
		Czy�ci list� u�ytkownik�w w danej grupie.
	**/
	void clear();

public slots:
	/**
		\fn void addUser(UserListElement ule, bool massively = false, bool last = false)
		\param ule obiekt reprezentuj�cy kontakt, kt�ry chcemy doda�.
		\param massively true, gdy jest to cze�� wi�kszego dodawania
		\param last true, gdy massively == true i jest to ostatnie dodanie
		Dodaje do listy podany kontakt.
	**/
	void addUser(const UserListElement &ule, bool massively = false, bool last = false);

	/**
		\fn void removeUser(UserListElement ule, bool massively = false, bool last = false)
		\param ule obiekt reprezentuj�cy kontakt, kt�ry chcemy usun��.
		\param massively true, gdy jest to cze�� wi�kszego usuwania
		\param last true, gdy massively == true i jest to ostatnie usuni�cie
		Usuwa podany kontakt z listy.
	**/
	void removeUser(const UserListElement &ule, bool massively = false, bool last = false);

	/**
		\fn UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false)
		\param protocolName identyfikator protoko�u
		\param id identyfikator kontaktu
		\param massively true, gdy jest to cze�� wi�kszego dodawania
		\param last true, gdy massively == true i jest to ostatnie dodanie
		Dodaje do listy anonimowy kontakt i zwraca go.
	**/
	UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false);

	/**
		\fn void addUsers(const UserGroup *group)
		Dodaje do listy wszystkie kontakty nale��ce do grupy group
		\param group grupa kontakt�w
	**/
	void addUsers(const UserGroup *group);

	/**
		\fn void addUsers(QValueList<UserListElement> users)
		Dodaje do listy wszystkie kontakty nale��ce do listy users
		\param users kontakty
	**/
	void addUsers(QList<UserListElement> users);

	/**
		\fn void removeUsers(const UserGroup *group)
		Usuwa z listy wszystkie kontakty nale��ce do grupy group
		\param group grupa kontakt�w
	**/
	void removeUsers(const UserGroup *group);

	/**
		\fn void removeUsers(QValueList<UserListElement> users)
		Usuwa z listy wszystkie kontakty nale��ce do listy users
		\param users kontakty
	**/
	void removeUsers(QList<UserListElement> users);

signals:
	/**
		\fn void modified()
		Sygna� generowany po zako�czeniu zmian w li�cie	kontakt�w.
		\todo sprawdzi� czy zawsze dzieje si� to po zako�czeniu zmian
	**/
	void modified();

	/**
		\fn void userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
		\param elem kontakt, kt�rego dane si� zmieni�y
		\param name nazwa w�asno�ci
		\param oldValue stara warto��
		\param currentValue nowa (bie��ca) warto��
		\param massively true, gdy jest to cze�� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		Sygna� generowany gdy dla kontaktu elem zmieni si� w�asno�� o nazwie name.
		Przekazuje tak�e star� i now� warto��.
	**/
	void userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);

	void usersDataChanged(QString name);

	/**
		\fn void protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
		\param protocolName identyfikator protoko�u
		\param elem kontakt, kt�rego dane si� zmieni�y
		\param name nazwa w�asno�ci
		\param oldValue stara warto��
		\param currentValue nowa (bie��ca) warto��
		\param massively true, gdy jest to cze�� wi�kszych zmian
		\param last true, gdy massively == true i jest to ostatnia zmiana
		Sygna� generowany gdy dla kontaktu elem zmieni si� w�asno�� zwi�zana z protoko�em
		protocolName o nazwie name. Przekazuje tak�e star� i now� warto��.
	**/
	void protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);

	void protocolUsersDataChanged(QString protocolName, QString name);

	/**
		\fn void userAdded(UserListElement elem, bool massively, bool last)
		\param elem dodany kontakt
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany po dodaniu kontaktu elem do listy
	**/
	void userAdded(UserListElement elem, bool massively, bool last);

	/**
		\fn void addingUser(UserListElement elem, bool massively, bool last)
		\param elem dodany kontakt
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany przed dodaniem kontaktu elem do listy
	**/
	void addingUser(UserListElement elem, bool massively, bool last);

	void usersAdded();

	/**
		\fn void removingUser(UserListElement elem, bool massively, bool last)
		\param elem usuwany kontakt
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany przed usuni�ciem kontaktu z listy.
	**/
	void removingUser(UserListElement elem, bool massively, bool last);

	/**
		\fn void userRemoved(UserListElement elem, bool massively, bool last)
		\param elem usuwany kontakt
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany po usuni�ciu kontaktu z listy.
	**/
	void userRemoved(UserListElement elem, bool massively, bool last);

	void usersRemoved();

	/**
		\fn void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last)
		\param elem kontakt, dla kt�rego dodany zosta� protok��
		\param protocolName identyfikator protoko�u
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany po dodaniu protoko�u protocolName kontaktowi elem.
	**/
	void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);

	/**
		\fn void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last)
		\param elem kontakt, dla kt�rego usuni�ty zosta� protok��
		\param protocolName identyfikator protoko�u
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		Sygna� generowany przed usuni�ciem protoko�u protocolName kontaktu elem.
	**/
	void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

	/**
		\fn void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last)
		\param elem kontakt, dla kt�rego zmieni� si� status
		\param protocolName identyfikator protoko�u
		\param oldStatus poprzedni status
		\param massively czy jest to fragment wi�kszych zmian
		\param last je�eli massively == true, to last == true dla ostatniego kontaktu
	**/
	void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

	void usersStatusChanged(QString protocolName);

};

/**
	\class UserListElements
	Prosta lista u�ytkownik�w z kilkoma u�atwiaczami.
**/
class KADUAPI UserListElements : public QList<UserListElement>
{
public:
	/**
		\fn UserListElements(UserListElement)
		Konstruktor dodaj�cy od razu wskazany kontakt
	**/
	UserListElements(UserListElement);

	/**
		\fn UserListElements(const UserListElements &)
		Konstruktor kopiuj�cy
	**/
	UserListElements(const UserListElements &);

	/**
		\fn UserListElements(const QValueList<UserListElement> &)
		Konstruktor inicjuj�cy si� wskazan� list�
	**/
	UserListElements(const QList<UserListElement> &);

	/**
		\fn UserListElements()
		Standardowy konstruktor.
	**/
	UserListElements();

	/**
		\fn bool equals(const UserListElements &elems) const
		Sprawdza czy bie��ca lista zaiwera te same elemnty co wskazana.
		\attention {Z�o�ono�� O(count()*elems.count()).}
	**/
	bool equals(const UserListElements &elems) const;

	/**
		\fn bool equals(const UserGroup *group) const
		Sprawdza czy bie��ca lista zaiwera te same elemnty co wskazana.
		\attention {Z�o�ono�� O(count()).}
	**/
	bool equals(const UserGroup *group) const;

	/**
		\fn void sort()
		Sortuje list� w spos�b jednoznaczny.
	**/
	void sort();

	/**
		\fn bool contains(QString protocol, QString id) const
		Zwraca informacj� o tym czy lista zawiera kontakt o wskazanym protokole i identyfiaktorze.
		(Z�o�ono�� O(n))
	**/
	bool contains(QString protocol, QString id) const;

	/**
		\fn bool contains(UserListElement e) const
		Zwraca informacj� o tym czy lista zawiera wskazany kontakt.
		(Z�o�ono�� O(n))
	**/
	bool contains(UserListElement e) const { return QList<UserListElement>::contains(e) > 0;}

	/**
		\fn QStringList altNicks() const
		Zwraca list� nick�w kontakt�w nale��cych w postaci obiektu
		klasy QStringList.
	**/
	QStringList altNicks() const;

	bool operator < (const UserListElements &) const;

};

#endif
