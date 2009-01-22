/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_USERBOX_H
#define KADU_USERBOX_H

#include <Qt3Support/Q3ListBoxItem>
#include <QtCore/QTimer>
#include <QtGui/QDrag>

#include "configuration_aware_object.h"
#include "misc.h"
#include "protocols/protocol.h"
#include "userlist.h"

#include "exports.h"

class QFontMetrics;

class ActionDescription;
class KaduMainWindow;
class ULEComparer;

class KADUAPI UlesDrag : public QDrag
{

public:
	UlesDrag(const QStringList &ules, QWidget* dragSource = 0);
	static bool decode(QDropEvent *event, QStringList &ules);
	static bool canDecode(QDragEnterEvent *event);

};

/**
	Klasa zdolna do pokazywania ToolTip�w nad oknem kontakt�w.
 **/
class ToolTipClass
{

public:

	ToolTipClass() {};
	virtual ~ToolTipClass() {};

	/**
		Pokazuje tooltip w punkcie point na temat kontaktu user.

		@param point punkt, w kt�rym ma si� pojawi� tooltip
		@param user u�ytkownik, kt�rego dotyczy informacja
	 **/
	virtual void showToolTip(const QPoint &point, Contact user) = 0;
	/**
		Ukrywa tooltip (je�eli by� pokazany)
	 **/
	virtual void hideToolTip() = 0;

};

/**
	Menad�er klas wy�wietlaj�cych tooltipy. Rejestruje klasy i pozwala na wyb�r jednej z nich w oknie konfiguracyjnym,
 **/
class KADUAPI ToolTipClassManager
{
	QMap<QString, ToolTipClass *> ToolTipClasses;
	QString ToolTipClassName;
	ToolTipClass *CurrentToolTipClass;

public:

	ToolTipClassManager();
	virtual ~ToolTipClassManager();

	/**
		Rejestruje now� klas� do pokazywania tooltip�w pod odpowiedni� nazw�.
		Zarejestrowane klasy pojawiaj� si� w oknie konfiguracji.

		@param toolTipClassName nazwa rejestrowanej klasy
		@param toolTipClass obiekt rejestrowanej klasy
	 **/
	void registerToolTipClass(const QString &toolTipClassName, ToolTipClass *toolTipClass);
	/**
		Wyrejestrowuje klas� do pokazywania tooltip�w pod odpowiedni� nazw�.

		@param toolTipClassName nazwa wyrejestrowanej klasy
	 **/
	void unregisterToolTipClass(const QString &toolTipClassName);

	/**
		Zwraca list� nazw zarejestrowanych klas do pokazywania tooltip�w.
	 **/
	QStringList getToolTipClasses();
	/**
		Wybiera klas�, kt�ra ma przej�� wy�wietlanie tooltip�w.
	 **/
	void useToolTipClass(const QString &toolTipClassName);

	/**
		Pokazuje tooltip w punkcie point na temat kontaktu user.

		@param point punkt, w kt�rym ma si� pojawi� tooltip
		@param user u�ytkownik, kt�rego dotyczy informacja
		@return true, gdy tooltip zosta� pokazany (klasa do obs�ugi jest za�adowana)
	 **/
	bool showToolTip(const QPoint &point, Contact contact);
	/**
		Ukrywa tooltip (je�eli by� pokazany)

		@return true, gdy tooltip zosta� ukryty (klasa do obs�ugi jest za�adowana)
	 **/
	bool hideToolTip();

};

/**
	Klasa reprezentuj�ca list� kontakt�w wraz z ikonkami stan�w.
	\class UserBox
	\brief Wy�wietlana lista kontakt�w.
**/
class KADUAPI UserBox : public Q3ListBox, ConfigurationAwareObject
{
	Q_OBJECT

private:
	friend class Kadu;

	KaduMainWindow *MainWindow; // TODO: !!!HACK!!!

	static QList<UserBox*> UserBoxes;
	bool fancy;

	ActionDescription *showDescriptionAction;

	ContactList Contacts;

// 	UserGroup *VisibleUsers;
// TOOD: 0.6.6
// 	QList<UserGroup *> Filters;
// 	QList<UserGroup *> NegativeFilters;
	std::vector<Contact> sortHelper;
	std::vector<Contact> toRemove;
	QMap<const UserGroup *, UserListElements> AppendProxy;
	QMap<const UserGroup *, UserListElements> RemoveProxy;

	ULEComparer *comparer;
	void sort();
	QTimer refreshTimer;

	Contact lastMouseStopContact;
	QTimer tipTimer;

	QTimer verticalPositionTimer;
	int lastVerticalPosition;

	QPoint MouseStart;

	static QImage *backgroundImage;
	void refreshBackground();

private slots:
	void accountRegistered(Account *account);
	void accountUnregistered(Account *account);

	void doubleClickedSlot(Q3ListBoxItem *item);
	void returnPressedSlot(Q3ListBoxItem *item);
	void currentChangedSlot(Q3ListBoxItem *item);

// 	void userAddedToVisible(UserListElement elem, bool massively, bool last);
// 	void userRemovedFromVisible(UserListElement elem, bool massively, bool last);

// 	void userAddedToGroup(UserListElement elem, bool massively, bool last);
// 	void userRemovedFromGroup(UserListElement elem, bool massively, bool last);

	void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
						QVariant currentValue, bool massively, bool last);
	void protocolUserDataChanged(QString protocolName, UserListElement elem,
						QString name, QVariant oldValue, QVariant currentValue,
						bool massively, bool last);
	void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

	void tipTimeout();
	void restartTip(const QPoint &p);
	void hideTip(bool waitForAnother = true);
	void resetVerticalPosition();
	void rememberVerticalPosition();

	void messageFromUserAdded(Contact elem);
	void messageFromUserDeleted(Contact elem);

	void selectionChangedSlot();

protected:
	virtual void configurationUpdated();

	virtual void wheelEvent(QWheelEvent *e);
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);

	/**
		\fn virtual void mousePressEvent(QMouseEvent *e)
		Wci�ni�to kt�ry� z przycisk�w myszki na li�cie kontakt�w.
		\param e wska�nik obiektu opisuj�cego to zdarzenie.
	**/
	virtual void mousePressEvent(QMouseEvent *e);

	virtual void mouseReleaseEvent(QMouseEvent *e);

	/**
		\fn virtual void mouseMoveEvent(QMouseEvent* e)
		Poruszono myszk� nad list� kontakt�w.
		\param e wska�nik obiektu opisuj�cego to zdarzenie.
	**/
	virtual void mouseMoveEvent(QMouseEvent* e);

	/**
		\fn virtual void keyPressEvent(QKeyEvent *e)
		Wci�ni�to kt�ry� z klawisz�w w aktywnej li�cie kontakt�w.
		\param e wska�nik obiektu opisuj�cego to zdarzenie.
	**/
	virtual void keyPressEvent(QKeyEvent *e);

	/**
		\fn virtual void resizeEvent(QResizeEvent *)
		Lista kontakt�w zmieni�a sw�j rozmiar.
		\param e wska�nik obiektu opisuj�cego to zdarzenie.
	**/
	virtual void resizeEvent(QResizeEvent *);

public:
	/**
		\fn UserBox(UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0)
		Standardowy konstruktor tworz�cy list� kontakt�w.
		\param group grupa kontakt�w, kt�ra b�dzie wy�wietlana
		\param parent rodzic kontrolki. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
		\param f flagi kontrolki. Domy�lnie 0.
	**/
	UserBox(KaduMainWindow *mainWindow, bool fancy, ContactList contacts, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);

	virtual ~UserBox();

	static void setColorsOrBackgrounds();

	/**
		\fn static void initModule()
		Inicjalizuje zmienne niezb�dne do dzia�ania UserBox. Funkcja ta jest
		wywo�ywana przy starcie Kadu przez rdze� Kadu.
	**/
	static void initModule();

	static void closeModule();

	/**
		\fn UserListElements getSelectedUsers() const
		Funkcja zwraca list� zaznaczonych uzytkownik�w.
		\return UserListElements z zaznaczonymi u�ytkownikami.
	**/
	UserListElements selectedUsers() const;

	/**
		\fn const UserGroup *visibleUsers() const
		zwraca list� kontakt�w, kt�re s� obecnie na li�cie
		\return grupa kontakt�w
	**/
	const UserGroup *visibleUsers() const;

	/**
		\fn QValueList<UserGroup *> filters() const
		Zwraca list� filtr�w "pozytywnych"
		\return lista filtr�w
	**/
	QList<UserGroup *> filters() const;

	/**
		\fn QValueList<UserGroup *> negativeFilters() const
		Zwraca list� filtr�w "negatywnych"
		\return lista filtr�w
	**/
	QList<UserGroup *> negativeFilters() const;

	/**
		\fn bool currentUserExists() const
		\return informacja o istnieniu bie��cego kontaktu
	**/
	bool currentUserExists() const;

	/**
		\fn UserListElement currentUser() const
		\attention {przed wywo�aniem tej metody nale�y sprawdzi�, czy istnieje bie��cy kontakt!}
		\return bie��cy kontakt
		\see UserBox::currentUserExists()
	**/
	Contact currentContact() const;

	class CmpFuncDesc
	{
		public:
			CmpFuncDesc(QString i, QString d, int (*f)(const Contact &, const Contact &))
				: id(i), description(d), func(f) {}
			CmpFuncDesc() : id(), description(), func(0){}

			QString id;
			QString description;
			int (*func)(const Contact &, const Contact &);

			bool operator == (const CmpFuncDesc &compare) { return id == compare.id; }
	};

	/**
		\fn QValueList<UserBox::CmpFuncDesc> compareFunctions() const
		\return lista obiekt�w opisuj�cych funkcje por�wnuj�ce
	**/
		QList<UserBox::CmpFuncDesc> compareFunctions() const;

	/**
		\fn void addCompareFunction(const QString &id, const QString &trDescription, int (*cmp)(const UserListElement &, const UserListElement &))
		Dodaje funkcj� por�wnuj�c� do bie��cego UserBoksa.
		Funkcja cmp powinna zwraca� warto��:
			< 0 gdy pierwszy kontakt powinien znale�� si� przed drugim
			= 0 gdy kolejno�� kontakt�w nie ma znaczenia
			> 0 gdy pierwszy kontakt powinien znale�� si� za drugim
		\param id kr�tki identyfikator funkcji
		\param trDescription pzet�umaczony opis funkcji (w zamierzeniu do wy�wietlenia w konfiguracji)
		\param cmp funkcja por�wnuj�ca
	**/
	void addCompareFunction(const QString &id, const QString &trDescription,
				int (*cmp)(const Contact &, const Contact &));

	/**
		\fn static void refreshAll()
		Od�wie�a wszystkie UserBoksy.
	**/
	static void refreshAll();

	/**
		\fn static void refreshAllLater()
		Od�wie�a wszystkie UserBoksy, ale dopiero po powrocie do p�tli zdarze� Qt.
	**/
	static void refreshAllLater();

	static const QList<UserBox *> &userboxes() {return UserBoxes;}

	static CreateNotifier createNotifier;

	KaduMainWindow * mainWindow() {return MainWindow;}

public slots:
	/**
		\fn void showDescriptionsActionActivated(QAction *sender, bool toggle);
		Slot jest wywo�ywany, gdy aktywowano ukrywanie opis�w kontakt�w
		\param users u�ytkownicy (nieu�ywany)
		\param widget (nieu�ywany)
		\param toggle w��czenie / wy��czenie ukrywania opis�w
	**/
	void showDescriptionsActionActivated(QAction *sender, bool toggle);

	/**
		\fn void setDescriptionsActionState()
		Slot jest wywo�ywany, w celu ustawienia poprawnego stanu dla akcji
		w��czaj�cej / wy��czaj�cej pokazywanie opis�w na li�cie kontakt�w
	**/
	void setDescriptionsActionState();

	/**
		\fn void applyFilter(UserGroup *group)
		Nak�ada na bie��cy UserBox filtr "pozytywny" - wy�wietlane b�d�
		tylko te kontakty, kt�re nale�� do group.
		\param group filtr
	**/
// 	void applyFilter(UserGroup *group, bool forceRefresh = true);

	/**
		\fn void removeFilter(UserGroup *group)
		Usuwa z bie��cego UserBoksa wskazany filtr "pozytywny".
		\param group filtr
	**/
// 	void removeFilter(UserGroup *group, bool forceRefresh = true);

	/**
		\fn void applyNegativeFilter(UserGroup *group)
		Nak�ada na bie��cy UserBox filtr "negatywny" - wy�wietlane b�d�
		tylko te kontakty, kt�re nie nale�� do group.
		\param group filtr
	**/
// 	void applyNegativeFilter(UserGroup *group, bool forceRefresh = true);

	/**
		\fn void removeNegativeFilter(UserGroup *group)
		Usuwa z bie��cego UserBoksa wskazany filtr "negatywny".
		\param group filtr
	**/
// 	void removeNegativeFilter(UserGroup *group, bool forceRefresh = true);

	/**
		\fn void removeCompareFunction(const QString &id)
		Usuwa funkcj� por�wnuj�c� o identyfikatorze id.
		\param id identyfikator funkcji por�wnuj�cej
	**/
	void removeCompareFunction(const QString &id);

	/**
		\fn void moveUpCompareFunction(const QString &id)
		Przesuwa funkcj� por�wnuj�c� o identyfikatorze id wy�ej w kolejno�ci sprawdzania.
		\param id identyfikator funkcji por�wnuj�cej
	**/
	bool moveUpCompareFunction(const QString &id);

	/**
		\fn void moveDownCompareFunction(const QString &id)
		Przesuwa funkcj� por�wnuj�c� o identyfikatorze id ni�ej w kolejno�ci sprawdzania.
		\param id identyfikator funkcji por�wnuj�cej
	**/
	bool moveDownCompareFunction(const QString &id);

	/**
		\fn void refresh()
		Od�wie�a bie��cy UserBox. Nie nale�y tej funkcji nadu�ywa�,
		bo wi�kszo�� (90%) sytuacji jest wykrywanych przez sam� klas�.
	**/
	void refresh();

	void refreshLater();

signals:
	/**
		\fn void doubleClicked(UserListElement user)
		Sygna� jest emitowany, gdy na kt�rym� z kontakt�w klikni�to dwukrotnie.
		\param user kontakt, na kt�rym kilkni�to dwukrotnie
		\warning U�ywaj tego sygna�u zamiast QListBox::doubleClicked(QListBoxItem *) !!!
		Tamten ze wzgl�du na od�wie�anie listy w jednym ze slot�w pod��czonych
		do tego sygna�u czasami przekazuje wska�nik do elementu, kt�ry ju� NIE ISTNIEJE.
	**/
	void doubleClicked(Contact contact);

	/**
		\fn void returnPressed(UserListElement user)
		Sygna� jest emitowany, gdy wci�ni�to klawisz enter na wybranym kontakcie.
		\param user kontakt, na kt�rym wci�ni�to enter
		\warning U�ywaj tego sygna�u zamiast QListBox::returnPressed(QListBoxItem *) !!!
		Tamten ze wzgl�du na od�wie�anie listy w jednym ze slot�w pod��czonych
		do tego sygna�u czasami przekazuje wska�nik do elementu, kt�ry ju� NIE ISTNIEJE.
	**/
	void returnPressed(Contact contact);

	/**
		\fn void currentChanged(UserListElement user)
		Sygna� jest emitowany, gdy zmieni� si� bie��cy kontakt.
		\attention {raczej nale�u u�ywa� tego sygna�u zamiast QListBox::currentChaned(QListBoxItem *)}
		\param user obecnie bie��cy kontakt
	**/
	void currentChanged(Contact CurrentContact);

	void userListChanged();

};

/**
	\fn int compareAltNick(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca AltNicki metod� QString::localeAwareCompare()
**/
int compareAltNick(const Contact &c1, const Contact &c2);

/**
	\fn int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca AltNicki metod� QString::localeAwareCompare(), wcze�niej robi�ca lower(),
	a wi�c wolniejsza od compareAltNick
**/
int compareAltNickCaseInsensitive(const Contact &c1, const Contact &c2);

/**
	\fn int compareStatus(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca statusy w protokole Gadu-Gadu. Uwa�a status "dost�pny" i "zaj�ty" za r�wnowa�ne.
**/
int compareStatus(const Contact &c1, const Contact &c2);

extern KADUAPI ToolTipClassManager *tool_tip_class_manager;

#endif
