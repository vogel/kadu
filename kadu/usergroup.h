#ifndef KADU_USERGROUP_H
#define KADU_USERGROUP_H

#include <qmap.h>
#include <qobject.h>
#include <qshared.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include "userlistelement.h"

class UserGroupData;
class UserListElements;

enum BehaviourForAnonymous {TrueForAnonymous, FalseForAnonymous};

/**
	\class UserGroup

	Ze wzglêdu na budowê tej klasy, parametr "last" w sygna³ach dostêpnych
	w tej klasie, mo¿e byæ u¿ywany tylko gdy mamy absolutn± pewno¶æ, ¿e ¿aden
	sygna³ nie utknie gdzie¶ we wnêtrzach Kadu. Dlatego te¿ bezpieczne jest
	u¿ywanie tego parametru tylko w slotach pod³±czonych do klasy UserList.
	Je¿eli mimo to potrzebujemy skorzystaæ z parametru massively w celu wykonania
	jakiej¶ czynno¶ci dopiero po zakoñczeniu zmian, mo¿emy pos³u¿yæ siê QTimerem
	z czasem 0 (czyli po oddaniu sterowania pêtli zdarzeñ Qt) i jednokrotnym
	uruchomieniem. Przyk³ad:
	<code>
	MojaKlasa{
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
		...//bardzo kosztowne od¶wie¿anie
	}

	void MojaKlasa::userAdded(UserListElement elem, bool massively, bool last)
	{
		if (massively)
		{
			akcja1; //np jakie¶ buforowanie
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
class UserGroup : public QObject
{
	Q_OBJECT

	public:
		typedef QValueListConstIterator<UserListElement> const_iterator;
		typedef QValueListIterator<UserListElement> iterator;
		typedef size_t size_type;

		/**
			\fn UserGroup(int size, const char *name = 0)
			\param size spodziewana ilo¶æ elementów * 2
			\param name nazwa grupy
		**/
		UserGroup(int size, const char *name = 0);

		/**
			\fn virtual ~UserGroup()
			Zwalnia pamiêæ zwi±zan± z grup±
		**/
		virtual ~UserGroup();

		/**
			\fn bool equals(UserListElements users) const
			\param users
			Z³o¿ono¶æ O(users.count()).
		**/
		bool equals(UserListElements users) const;

		/**
			\fn bool equals(UserListElements users) const
			\param users
			Z³o¿ono¶æ O(users.count()).
		**/
		bool equals(const UserGroup *group) const;

		/**
			\fn UserListElement byID(const QString &protocolName, const QString &id)
			\param protocolName identyfikator protoko³u
			\param id identyfikator kontaktu
			Zwraca element listy kontaktów identyfikowany przez nazwê
			protoko³u i identyfikator w tym¿e protokole.
			Je¿eli nie znajdzie siê taki kontakt, to tworzony jest nowy
			anonimowy kontakt i dodawany do listy.
			Z³o¿ono¶æ O(count()), ale w przysz³o¶ci bêdzie optymalizowana.
		**/
		UserListElement byID(const QString &protocolName, const QString &id);

		/**
			\fn UserListElement byAltNick(const QString &altnick)
			\param altnick wy¶wietlany pseudonim wg. którego nastêpuje wyszukiwanie.
			\return obiekt reprezentuj±cy kontakt o podanym pseudonimie wy¶wietlanym.
			Wyszukuje kontakt po wy¶wietlanym pseudonimie.
			Je¿eli nie znajdzie siê taki kontakt, to tworzony jest nowy
			anonimowy kontakt i dodawany do listy.
			Z³o¿ono¶æ O(count()), ale w przysz³o¶ci bêdzie optymalizowana do O(1).
		**/
		UserListElement byAltNick(const QString &altnick);

		/**
			\fn bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param protocolName identyfikator protoko³u
			\param id identyfikator kontaktu
			\param beh sposób zachowania dla anonimów
			Sprawdza czy grupa zawiera kontakt o danym identyfikatorze we wskazanym protokole.
			W przypadku kontaktów anonimowych zachowanie tej funkcji okre¶la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy¶lnie), to zwraca prawdê
				FalseForAnonymous, to zwraca fa³sz
			Z³o¿ono¶æ O(count()), ale w przysz³o¶ci bêdzie optymalizowana.
		**/
		bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn bool contains(UserListElement elem, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param elem sprawdzany kontakt
			\param beh sposób zachowania dla anonimów
			Sprawdza czy grupa zawiera wskazany kontakt.
			W przypadku kontaktów anonimowych zachowanie tej funkcji okre¶la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy¶lnie), to  zwraca prawdê
				FalseForAnonymous, to zwraca fa³sz
			Z³o¿ono¶æ O(1).
		**/
		bool contains(UserListElement elem, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param altnick wy¶wietlany pseudonim
			\param beh sposób zachowania dla anonimów
			Sprawdza czy grupa zawiera kontakt o danym pseudonimie wy¶wietlanym.
			W przypadku kontaktów anonimowych zachowanie tej funkcji okre¶la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy¶lnie), to  zwraca prawdê
				FalseForAnonymous, to zwraca fa³sz
			Z³o¿ono¶æ O(count()), ale w przysz³o¶ci bêdzie optymalizowana do O(1).
		**/
		bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn size_type count() const
			Zwraca ilo¶æ kontaktów.
		**/
		size_type count() const;

		/**
			\fn const_iterator constBegin () const
			Zwraca iterator pocz±tkowy.
		**/
		const_iterator constBegin () const;

		/**
			\fn const_iterator constEnd () const
			Zwraca iterator koñcowy.
		**/
		const_iterator constEnd () const;

		/**
			\fn const_iterator begin () const
			Zwraca iterator pocz±tkowy.
		**/
		iterator begin () const;

		/**
			\fn const_iterator end () const
			Zwraca iterator koñcowy.
		**/
		iterator end () const;

		/**
			\fn UserListElements toUserListElements() const
			Zwraca listê kontaktów w postaci obiektu klasy UserListElements
		**/
		UserListElements toUserListElements() const;

		/**
			\fn void resize(int size)
			\param size rozmiar bufora * 2
			Zmienia wewnêtrzny rozmiar buforów dla klas s³ownikowych (Q*Dict).
		**/
		void resize(int size);

	public slots:
		/**
			\fn void addUser(UserListElement ule, bool massively = false, bool last = false)
			\param ule obiekt reprezentuj±cy kontakt, który chcemy dodaæ.
			\param massively true, gdy jest to cze¶æ wiêkszego dodawania
			\param last true, gdy massively == true i jest to ostatnie dodanie
			Dodaje do listy podany kontakt.
		**/
		void addUser(UserListElement ule, bool massively = false, bool last = false);

		/**
			\fn void removeUser(UserListElement ule, bool massively = false, bool last = false)
			\param ule obiekt reprezentuj±cy kontakt, który chcemy usun±æ.
			\param massively true, gdy jest to cze¶æ wiêkszego usuwania
			\param last true, gdy massively == true i jest to ostatnie usuniêcie
			Usuwa podany kontakt z listy.
		**/
		void removeUser(UserListElement ule, bool massively = false, bool last = false);

		/**
			\fn UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko³u
			\param id identyfikator kontaktu
			\param massively true, gdy jest to cze¶æ wiêkszego dodawania
			\param last true, gdy massively == true i jest to ostatnie dodanie
			Dodaje do listy anonimowy kontakt i zwraca go.
		**/
		UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false);

		/**
			\fn void addUsers(const UserGroup *group)
			Dodaje do listy wszystkie kontakty nale¿±ce do grupy group
			\param group grupa kontaktów
		**/
		void addUsers(const UserGroup *group);

		/**
			\fn void addUsers(QValueList<UserListElement> users)
			Dodaje do listy wszystkie kontakty nale¿±ce do listy users
			\param users kontakty
		**/
		void addUsers(QValueList<UserListElement> users);

		/**
			\fn void removeUsers(const UserGroup *group)
			Usuwa z listy wszystkie kontakty nale¿±ce do grupy group
			\param group grupa kontaktów
		**/
		void removeUsers(const UserGroup *group);

		/**
			\fn void removeUsers(QValueList<UserListElement> users)
			Usuwa z listy wszystkie kontakty nale¿±ce do listy users
			\param users kontakty
		**/
		void removeUsers(QValueList<UserListElement> users);

	signals:
		/**
			\fn void modified()
			Sygna³ generowany po zakoñczeniu zmian w li¶cie	kontaktów.
			\todo sprawdziæ czy zawsze dzieje siê to po zakoñczeniu zmian
		**/
		void modified();

		/**
			\fn void userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
			\param elem kontakt, którego dane siê zmieni³y
			\param name nazwa w³asno¶ci
			\param oldValue stara warto¶æ
			\param currentValue nowa (bie¿±ca) warto¶æ
			\param massively true, gdy jest to cze¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Sygna³ generowany gdy dla kontaktu elem zmieni siê w³asno¶æ o nazwie name.
			Przekazuje tak¿e star± i now± warto¶æ.
		**/
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);

		/**
			\fn void protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
			\param protocolName identyfikator protoko³u
			\param elem kontakt, którego dane siê zmieni³y
			\param name nazwa w³asno¶ci
			\param oldValue stara warto¶æ
			\param currentValue nowa (bie¿±ca) warto¶æ
			\param massively true, gdy jest to cze¶æ wiêkszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Sygna³ generowany gdy dla kontaktu elem zmieni siê w³asno¶æ zwi±zana z protoko³em
			protocolName o nazwie name. Przekazuje tak¿e star± i now± warto¶æ.
		**/
		void protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);

		/**
			\fn void userAdded(UserListElement elem, bool massively, bool last)
			\param elem dodany kontakt
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany po dodaniu kontaktu elem do listy
		**/
		void userAdded(UserListElement elem, bool massively, bool last);

		/**
			\fn void addingUser(UserListElement elem, bool massively, bool last)
			\param elem dodany kontakt
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany przed dodaniem kontaktu elem do listy
		**/
		void addingUser(UserListElement elem, bool massively, bool last);

		/**
			\fn void removingUser(UserListElement elem, bool massively, bool last)
			\param elem usuwany kontakt
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany przed usuniêciem kontaktu z listy.
		**/
		void removingUser(UserListElement elem, bool massively, bool last);

		/**
			\fn void userRemoved(UserListElement elem, bool massively, bool last)
			\param elem usuwany kontakt
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany po usuniêciu kontaktu z listy.
		**/
		void userRemoved(UserListElement elem, bool massively, bool last);

		/**
			\fn void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last)
			\param elem kontakt, dla którego dodany zosta³ protokó³
			\param protocolName identyfikator protoko³u
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany po dodaniu protoko³u protocolName kontaktowi elem.
		**/
		void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);

		/**
			\fn void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last)
			\param elem kontakt, dla którego usuniêty zosta³ protokó³
			\param protocolName identyfikator protoko³u
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
			Sygna³ generowany przed usuniêciem protoko³u protocolName kontaktu elem.
		**/
		void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

		/**
			\fn void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last)
			\param elem kontakt, dla którego zmieni³ siê status
			\param protocolName identyfikator protoko³u
			\param oldStatus poprzedni status
			\param massively czy jest to fragment wiêkszych zmian
			\param last je¿eli massively == true, to last == true dla ostatniego kontaktu
		**/
		void statusChanged(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);

	protected:
		UserGroupData *d;
		friend class UserListElement;
		friend class ULEPrivate;

		/**
			\fn UserListElement byKey(UserListKey key)
			\param key klucz
			Zwraca element listy kontaktów identyfikowany przez klucz.
		**/
		UserListElement byKey(UserListKey key);

	private:
		/* standardowy konstruktor kopiuj±cy zepsu³by obiekt ¼ród³owy */
		UserGroup(const UserGroup &) {}

		/* standardowy operator kopiowania zepsu³by obiekt ¼ród³owy */
 		virtual void operator = (const UserGroup &) {}

		/* nie da siê porównywaæ obiektów tej klasy */
		bool operator==(const UserGroup &) const {return false;}

};

/**
	\class UserListElements
	Prosta lista u¿ytkowników z kilkoma u³atwiaczami.
**/
class UserListElements : public QValueList<UserListElement>
{
	public:
		/**
			\fn UserListElements(UserListElement)
			Konstruktor dodaj±cy od razu wskazany kontakt
		**/
		UserListElements(UserListElement);

		/**
			\fn UserListElements()
			Standardowy konstruktor.
		**/
		UserListElements();

		/**
			\fn bool equals(const UserListElements &elems) const
			Sprawdza czy bie¿±ca lista zaiwera te same elemnty co wskazana.
			\attention {Z³o¿ono¶æ O(count()*elems.count()).}
		**/
		bool equals(const UserListElements &elems) const;

		/**
			\fn bool equals(const UserGroup *group) const
			Sprawdza czy bie¿±ca lista zaiwera te same elemnty co wskazana.
			\attention {Z³o¿ono¶æ O(count()).}
		**/
		bool equals(const UserGroup *group) const;

		/**
			\fn void sort()
			Sortuje listê w sposób jednoznaczny.
		**/
		void sort();

		/**
			\fn bool contains(QString protocol, QString id) const
			Zwraca informacjê o tym czy lista zawiera kontakt o wskazanym protokole i identyfiaktorze.
			(Z³o¿ono¶æ O(n))
		**/
		bool contains(QString protocol, QString id) const;

		/**
			\fn bool contains(UserListElement e) const
			Zwraca informacjê o tym czy lista zawiera wskazany kontakt.
			(Z³o¿ono¶æ O(n))
		**/
		bool contains(UserListElement e) const { return QValueList<UserListElement>::contains(e) > 0;}
};

#endif
