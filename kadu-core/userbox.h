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
#include "protocol.h"
#include "userlist.h"

class QFontMetrics;

class ActionDescription;
class KaduMainWindow;
class ULEComparer;

class UlesDrag : public QDrag
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
	virtual void showToolTip(const QPoint &point, const UserListElement &user) = 0;
	/**
		Ukrywa tooltip (je�eli by� pokazany)
	 **/
	virtual void hideToolTip() = 0;

};

/**
	Menad�er klas wy�wietlaj�cych tooltipy. Rejestruje klasy i pozwala na wyb�r jednej z nich w oknie konfiguracyjnym,
 **/
class ToolTipClassManager
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
	bool showToolTip(const QPoint &point, const UserListElement &user);
	/**
		Ukrywa tooltip (je�eli by� pokazany)

		@return true, gdy tooltip zosta� ukryty (klasa do obs�ugi jest za�adowana)
	 **/
	bool hideToolTip();

};

/**
	Klasa reprezentuj�ca kontakt wizualnie na li�cie kontakt�w. Opisuje ona ikon� kontaktu,
	jego wy�wietlan� nazw� oraz ewentualny opis.
	\class KaduListBoxPixmap
	\brief Klasa reprezentuj�ca kontakt wizualnie.
**/
class KaduListBoxPixmap : public Q3ListBoxItem
{
	QPixmap pm;
	static QFontMetrics *descriptionFontMetrics;

	// TODO: co to do cholery robi tutaj?
	static UinType myUIN;
	static bool ShowDesc;
	static bool ShowBold;
	static bool AlignUserboxIconsTop;
	static bool ShowMultilineDesc;
	static bool MultiColumn;
	static int  MultiColumnWidth;
	static int ColumnCount;
	static QColor descColor;

	mutable QString buf_text;
	mutable int buf_width;
	mutable QStringList buf_out;
	mutable int buf_height;

protected:
	/**
		\fn int lineHeight(const QListBox *lb) const
		Zwraca wysoko�� samej czcionki dla elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
	**/
	int lineHeight(const Q3ListBox *lb) const;

	/**
		\fn void paint(QPainter *painter)
		Rysuje wy�wietlany kontakt na li�cie.
		\param painter wska�nik do obiektu rysuj�cego.
	**/
	void paint(QPainter *painter);

	/**
		\fn void calculateSize(const QString &text, int width, QStringList &out, int &height) const
		Oblicza odpowiedni� wysoko�� elementu listy kontakt�w tak, aby pomie�ci�
		opis podzielony na niezb�dn� liczb� linii.
		\param[in] text wy�wietlany opis.
		\param[in] width szeroko�� kolumny listy kontakt�w.
		\param[out] out lista �a�cuch�w znak�w, zawieraj�ca kolejne linie opisu powsta�e
			w wyniku podzielenia opisu tak, aby zmie�ci� si� w danej szeroko�ci kolumny.
		\param[out] height wysoko�� elementu listy kontakt�w potrzebna aby pomie�ci� opis
			podzielony w liniach.
	**/
	void calculateSize(const QString &text, int width, QStringList &out, int &height) const;

	/**
		\fn void changeText(const QString &text)
		Zmienia nazw� wy�wietlan� dla kontaktu na li�cie na podan�.
		\param text nowa nazwa do wy�wietlenia.
	**/
	void changeText(const QString &text);

	friend class UserBox;
	friend class Kadu; // TODO: fix

	static QPixmap pixmapForUser(const UserListElement &user);

	/*funkcje wprowadzone �eby zaoszcz�dzi� na odwo�aniach do pliku konfiguracyjnego*/
	/**
		\fn static void setFont(const QFont &f)
		Ustawia czcionk� dla tego elementu.
		\param f czcionka
	**/
	static void setFont(const QFont &f);

	/**
		\fn static void setMyUIN(UinType u)
		Ustawia UIN lokalnego u�ytkownika Kadu dla tego elementu, aby by�o wiadomo,
		�e ten element jego reprezentuje (poniewa� do pobierania informacji o statusie i opisie
		u�ywane s� r��ne metody dla kontaktu i lokalnego u�ytkownika).
		\param u numer UIN.
	**/
	static void setMyUIN(UinType u);

	/**
		\fn static void setShowDesc(bool sd)
		Ustawia stan wy�wietlania opis�w na li�cie kontakt�w.
		\param sd warto�� logiczna informuj�ca o tym, czy opisy maj� by� wy�wietlane na li�cie kontat�w.
	**/
	static void setShowDesc(bool sd);

	static void setShowBold(bool sb);

	/**
		\fn static void setAlignTop(bool at)
		Ustawia stan wyr�wnywania do g�rnego brzegu elementu.
		\param at warto�� logiczna informuj�ca o tym, czy wy�wietlana nazwa kontaktu ma by� wyr�wnywana
			do g�rnej kraw�dzi elementu.
	**/
	static void setAlignTop(bool at);

	/**
		\fn static void setShowMultilineDesc(bool m)
		Ustawia stan wy�wietlania opis�w wieloliniowych.
		\param m warto�� logiczna informuj�ca o tym, czy opis tego elementu mo�e by� wy�wietlany
			w wielu liniach, je�li zawiera znaki nowej linii.
	**/
	static void setShowMultilineDesc(bool m);

	/**
		\fn static void setMultiColumn(bool m)
		Ustawia stan wy�wietlania listy kontakt�w w wielu kolumnach.
		\param m warto�� logiczna informuj�ca o tym, czy lista ma by� wy�wietlana w wielu kolumnach.
	**/
	static void setColumnCount(int m);

	/**
		\fn static void setDescriptionColor(const QColor &col)
		Ustawia kolor wy�wietlanego opisu.
		\param col kolor
	**/
	static void setDescriptionColor(const QColor &col);

public:
	/**
		\fn KaduListBoxPixmap(const QPixmap &pix, UserListElement user, bool bold = false)
		Konstruktor dodaj�cy kontakt do listy.
		\param user kontakt, z kt�rego b�d� pobierane dane do wy�wietlenia
		\param bold warto�� logiczna informuj�ca o tym, czy nazwa kontaktu
			ma by� wy�wietlana pogrubion� czcionk�.
	**/
	KaduListBoxPixmap(UserListElement user);

	/**
		\fn bool isBold() const
		Zwraca informacj�, czy nazwa kontaktu jest wy�wietlana pogrubion� czcionk�, czy nie.
	**/
	bool isBold() const;

	/**
		\fn int height(const QListBox *lb) const
		Zwraca wysoko�� elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
	**/
	virtual int height(const Q3ListBox *lb) const;

	/**
		\fn int width(const QListBox *lb) const
		Zwraca szeroko�� elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
	**/
	virtual int width(const Q3ListBox *lb) const;

	/**
		\fn void refreshItem()
		Od�wie�a ikon� i tekst elementu.
	**/
	void refreshItem();

	const UserListElement User;

};

/**
	Klasa reprezentuj�ca list� kontakt�w wraz z ikonkami stan�w.
	\class UserBox
	\brief Wy�wietlana lista kontakt�w.
**/
class UserBox : public Q3ListBox, ConfigurationAwareObject
{
	Q_OBJECT

	static QList<ActionDescription *> UserBoxActions;
	static QList<ActionDescription *> ManagementActions;

	friend class Kadu;

	KaduMainWindow *MainWindow; // TODO: !!!HACK!!!

	static QList<UserBox*> UserBoxes;
	bool fancy;

	ActionDescription *showDescriptionAction;

	UserGroup *VisibleUsers;
	QList<UserGroup *> Filters;
	QList<UserGroup *> NegativeFilters;
	std::vector<UserListElement> sortHelper;
	std::vector<UserListElement> toRemove;
	QMap<const UserGroup *, UserListElements> AppendProxy;
	QMap<const UserGroup *, UserListElements> RemoveProxy;

	ULEComparer *comparer;
	void sort();
	QTimer refreshTimer;

	UserListElement lastMouseStopUser;
	static UserListElement nullElement;
	QTimer tipTimer;

	QTimer verticalPositionTimer;
	int lastVerticalPosition;

	static QImage *backgroundImage;
	void refreshBackground();

private slots:
	void doubleClickedSlot(Q3ListBoxItem *item);
	void returnPressedSlot(Q3ListBoxItem *item);
	void currentChangedSlot(Q3ListBoxItem *item);

	void userAddedToVisible(UserListElement elem, bool massively, bool last);
	void userRemovedFromVisible(UserListElement elem, bool massively, bool last);

	void userAddedToGroup(UserListElement elem, bool massively, bool last);
	void userRemovedFromGroup(UserListElement elem, bool massively, bool last);

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

	void messageFromUserAdded(UserListElement elem);
	void messageFromUserDeleted(UserListElement elem);

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

	virtual void contextMenuEvent(QContextMenuEvent *event);

public:
	/**
		\fn UserBox(UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0)
		Standardowy konstruktor tworz�cy list� kontakt�w.
		\param group grupa kontakt�w, kt�ra b�dzie wy�wietlana
		\param parent rodzic kontrolki. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
		\param f flagi kontrolki. Domy�lnie 0.
	**/
	UserBox(KaduMainWindow *mainWindow, bool fancy, UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);

	virtual ~UserBox();

	static void addActionDescription(ActionDescription *actionDescription);
	static void insertActionDescription(int pos, ActionDescription *actionDescription);
	static void removeActionDescription(ActionDescription *actionDescription);
	static void addSeparator();
	static void addManagementActionDescription(ActionDescription *actionDescription);
	static void insertManagementActionDescription(int pos, ActionDescription *actionDescription);
	static void removeManagementActionDescription(ActionDescription *actionDescription);
	static void addManagementSeparator();

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
	UserListElement currentUser() const;

	class CmpFuncDesc
	{
		public:
			CmpFuncDesc(QString i, QString d, int (*f)(const UserListElement &, const UserListElement &))
				: id(i), description(d), func(f) {}
			CmpFuncDesc() : id(), description(), func(0){}

			QString id;
			QString description;
			int (*func)(const UserListElement &, const UserListElement &);

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
				int (*cmp)(const UserListElement &, const UserListElement &));

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
	void applyFilter(UserGroup *group);

	/**
		\fn void removeFilter(UserGroup *group)
		Usuwa z bie��cego UserBoksa wskazany filtr "pozytywny".
		\param group filtr
	**/
	void removeFilter(UserGroup *group);

	/**
		\fn void applyNegativeFilter(UserGroup *group)
		Nak�ada na bie��cy UserBox filtr "negatywny" - wy�wietlane b�d�
		tylko te kontakty, kt�re nie nale�� do group.
		\param group filtr
	**/
	void applyNegativeFilter(UserGroup *group);

	/**
		\fn void removeNegativeFilter(UserGroup *group)
		Usuwa z bie��cego UserBoksa wskazany filtr "negatywny".
		\param group filtr
	**/
	void removeNegativeFilter(UserGroup *group);

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
	void doubleClicked(UserListElement user);

	/**
		\fn void returnPressed(UserListElement user)
		Sygna� jest emitowany, gdy wci�ni�to klawisz enter na wybranym kontakcie.
		\param user kontakt, na kt�rym wci�ni�to enter
		\warning U�ywaj tego sygna�u zamiast QListBox::returnPressed(QListBoxItem *) !!!
		Tamten ze wzgl�du na od�wie�anie listy w jednym ze slot�w pod��czonych
		do tego sygna�u czasami przekazuje wska�nik do elementu, kt�ry ju� NIE ISTNIEJE.
	**/
	void returnPressed(UserListElement user);

	/**
		\fn void currentChanged(UserListElement user)
		Sygna� jest emitowany, gdy zmieni� si� bie��cy kontakt.
		\attention {raczej nale�u u�ywa� tego sygna�u zamiast QListBox::currentChaned(QListBoxItem *)}
		\param user obecnie bie��cy kontakt
	**/
	void currentChanged(UserListElement user);

	void userListChanged();

};

/**
	\fn int compareAltNick(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca AltNicki metod� QString::localeAwareCompare()
**/
int compareAltNick(const UserListElement &u1, const UserListElement &u2);

/**
	\fn int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca AltNicki metod� QString::localeAwareCompare(), wcze�niej robi�ca lower(),
	a wi�c wolniejsza od compareAltNick
**/
int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2);

/**
	\fn int compareStatus(const UserListElement &u1, const UserListElement &u2)
	Funkcja por�wnuj�ca statusy w protokole Gadu-Gadu. Uwa�a status "dost�pny" i "zaj�ty" za r�wnowa�ne.
**/
int compareStatus(const UserListElement &u1, const UserListElement &u2);

extern ToolTipClassManager *tool_tip_class_manager;

#endif
