#ifndef KADU_USERBOX_H
#define KADU_USERBOX_H

#include <qlistbox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include <vector>

#include "configuration_aware_object.h"
#include "drag_simple.h"
#include "gadu.h"
#include "misc.h"
#include "userlistelement.h"

class QFontMetrics;
class ULEComparer;
class Action;

class UlesDrag : public DragSimple
{

public:
	UlesDrag(const QStringList &ules, QWidget* dragSource = 0, const char* name = 0);
	static bool decode(const QMimeSource *source, QStringList &ules);
	static bool canDecode(const QMimeSource *source);

};

/**
	Klasa zdolna do pokazywania ToolTipów nad oknem kontaktów.
 **/
class ToolTipClass
{

public:

	ToolTipClass() {};
	virtual ~ToolTipClass() {};

	/**
		Pokazuje tooltip w punkcie point na temat kontaktu user.

		@param point punkt, w którym ma siê pojawiæ tooltip
		@param user u¿ytkownik, którego dotyczy informacja
	 **/
	virtual void showToolTip(const QPoint &point, const UserListElement &user) = 0;
	/**
		Ukrywa tooltip (je¿eli by³ pokazany)
	 **/
	virtual void hideToolTip() = 0;

};

/**
	Menad¿er klas wy¶wietlaj±cych tooltipy. Rejestruje klasy i pozwala na wybór jednej z nich w oknie konfiguracyjnym,
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
		Rejestruje now± klasê do pokazywania tooltipów pod odpowiedni± nazw±.
		Zarejestrowane klasy pojawiaj± siê w oknie konfiguracji.

		@param toolTipClassName nazwa rejestrowanej klasy
		@param toolTipClass obiekt rejestrowanej klasy
	 **/
	void registerToolTipClass(const QString &toolTipClassName, ToolTipClass *toolTipClass);
	/**
		Wyrejestrowuje klasê do pokazywania tooltipów pod odpowiedni± nazw±.

		@param toolTipClassName nazwa wyrejestrowanej klasy
	 **/
	void unregisterToolTipClass(const QString &toolTipClassName);

	/**
		Zwraca listê nazw zarejestrowanych klas do pokazywania tooltipów.
	 **/
	QStringList getToolTipClasses();
	/**
		Wybiera klasê, która ma przej±æ wy¶wietlanie tooltipów.
	 **/
	void useToolTipClass(const QString &toolTipClassName);

	/**
		Pokazuje tooltip w punkcie point na temat kontaktu user.

		@param point punkt, w którym ma siê pojawiæ tooltip
		@param user u¿ytkownik, którego dotyczy informacja
		@return true, gdy tooltip zosta³ pokazany (klasa do obs³ugi jest za³adowana)
	 **/
	bool showToolTip(const QPoint &point, const UserListElement &user);
	/**
		Ukrywa tooltip (je¿eli by³ pokazany)

		@return true, gdy tooltip zosta³ ukryty (klasa do obs³ugi jest za³adowana)
	 **/
	bool hideToolTip();

};

/**
	Klasa reprezentuj±ca kontakt wizualnie na li¶cie kontaktów. Opisuje ona ikonê kontaktu,
	jego wy¶wietlan± nazwê oraz ewentualny opis.
	\class KaduListBoxPixmap
	\brief Klasa reprezentuj±ca kontakt wizualnie.
**/
class KaduListBoxPixmap : public QListBoxItem
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
		Zwraca wysoko¶æ samej czcionki dla elementu reprezentuj±cego wy¶wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj±cy wy¶wietlany kontakt.
	**/
	int lineHeight(const QListBox *lb) const;

	/**
		\fn void paint(QPainter *painter)
		Rysuje wy¶wietlany kontakt na li¶cie.
		\param painter wska¼nik do obiektu rysuj±cego.
	**/
	void paint(QPainter *painter);

	/**
		\fn void calculateSize(const QString &text, int width, QStringList &out, int &height) const
		Oblicza odpowiedni± wysoko¶æ elementu listy kontaktów tak, aby pomie¶ciæ
		opis podzielony na niezbêdn± liczbê linii.
		\param[in] text wy¶wietlany opis.
		\param[in] width szeroko¶æ kolumny listy kontaktów.
		\param[out] out lista ³añcuchów znaków, zawieraj±ca kolejne linie opisu powsta³e
			w wyniku podzielenia opisu tak, aby zmie¶ci³ siê w danej szeroko¶ci kolumny.
		\param[out] height wysoko¶æ elementu listy kontaktów potrzebna aby pomie¶ciæ opis
			podzielony w liniach.
	**/
	void calculateSize(const QString &text, int width, QStringList &out, int &height) const;

	/**
		\fn void changeText(const QString &text)
		Zmienia nazwê wy¶wietlan± dla kontaktu na li¶cie na podan±.
		\param text nowa nazwa do wy¶wietlenia.
	**/
	void changeText(const QString &text);

	friend class UserBox;

	static QPixmap pixmapForUser(const UserListElement &user);

	/*funkcje wprowadzone ¿eby zaoszczêdziæ na odwo³aniach do pliku konfiguracyjnego*/
	/**
		\fn static void setFont(const QFont &f)
		Ustawia czcionkê dla tego elementu.
		\param f czcionka
	**/
	static void setFont(const QFont &f);

	/**
		\fn static void setMyUIN(UinType u)
		Ustawia UIN lokalnego u¿ytkownika Kadu dla tego elementu, aby by³o wiadomo,
		¿e ten element jego reprezentuje (poniewa¿ do pobierania informacji o statusie i opisie
		u¿ywane s± ró¿ne metody dla kontaktu i lokalnego u¿ytkownika).
		\param u numer UIN.
	**/
	static void setMyUIN(UinType u);

	/**
		\fn static void setShowDesc(bool sd)
		Ustawia stan wy¶wietlania opisów na li¶cie kontaktów.
		\param sd warto¶æ logiczna informuj±ca o tym, czy opisy maj± byæ wy¶wietlane na li¶cie kontatów.
	**/
	static void setShowDesc(bool sd);

	static void setShowBold(bool sb);

	/**
		\fn static void setAlignTop(bool at)
		Ustawia stan wyrównywania do górnego brzegu elementu.
		\param at warto¶æ logiczna informuj±ca o tym, czy wy¶wietlana nazwa kontaktu ma byæ wyrównywana
			do górnej krawêdzi elementu.
	**/
	static void setAlignTop(bool at);

	/**
		\fn static void setShowMultilineDesc(bool m)
		Ustawia stan wy¶wietlania opisów wieloliniowych.
		\param m warto¶æ logiczna informuj±ca o tym, czy opis tego elementu mo¿e byæ wy¶wietlany
			w wielu liniach, je¶li zawiera znaki nowej linii.
	**/
	static void setShowMultilineDesc(bool m);

	/**
		\fn static void setMultiColumn(bool m)
		Ustawia stan wy¶wietlania listy kontaktów w wielu kolumnach.
		\param m warto¶æ logiczna informuj±ca o tym, czy lista ma byæ wy¶wietlana w wielu kolumnach.
	**/
	static void setColumnCount(int m);

	/**
		\fn static void setDescriptionColor(const QColor &col)
		Ustawia kolor wy¶wietlanego opisu.
		\param col kolor
	**/
	static void setDescriptionColor(const QColor &col);

public:
	/**
		\fn KaduListBoxPixmap(const QPixmap &pix, UserListElement user, bool bold = false)
		Konstruktor dodaj±cy kontakt do listy.
		\param user kontakt, z którego bêd± pobierane dane do wy¶wietlenia
		\param bold warto¶æ logiczna informuj±ca o tym, czy nazwa kontaktu
			ma byæ wy¶wietlana pogrubion± czcionk±.
	**/
	KaduListBoxPixmap(UserListElement user);

	/**
		\fn bool isBold() const
		Zwraca informacjê, czy nazwa kontaktu jest wy¶wietlana pogrubion± czcionk±, czy nie.
	**/
	bool isBold() const;

	/**
		\fn int height(const QListBox *lb) const
		Zwraca wysoko¶æ elementu reprezentuj±cego wy¶wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj±cy wy¶wietlany kontakt.
	**/
	virtual int height(const QListBox *lb) const;

	/**
		\fn int width(const QListBox *lb) const
		Zwraca szeroko¶æ elementu reprezentuj±cego wy¶wietlany kontakt w pikselach.
		\param lb obiekt reprezentuj±cy wy¶wietlany kontakt.
	**/
	virtual int width(const QListBox *lb) const;

	/**
		\fn void refreshItem()
		Od¶wie¿a ikonê i tekst elementu.
	**/
	void refreshItem();

	const UserListElement User;

};

/**
	Klasa reprezentuj±ca menu podrêczne listy kontaktów - czyli to, które dostajemy klikaj±c
	na którymkolwiek kontakcie prawym przyciskiem myszy.
	\class UserBoxMenu
	\brief Menu podrêczne listy kontaktów.
**/
class UserBoxMenu : public QPopupMenu
{
	Q_OBJECT

	private:
		QValueList<QPair<QString, QString> > iconNames;

	private slots:
		void restoreLook();

	public:
		/**
			\fn UserBoxMenu(QWidget *parent=0, const char* name=0)
			Standardowy konstruktor.
			\param parent wska¼nik na obiekt kontrolki-rodzica.
			\param name nazwa kontrolki.
		**/
		UserBoxMenu(QWidget *parent=0, const char* name=0);

		/**
			\fn int getItem(const QString &caption) const
			Zwraca unikaln± liczbê identyfikuj±c± element menu z podanym napisem.
			\param caption napis elementu.
		**/
		int getItem(const QString &caption) const;

	public slots:
		/**
			\fn void show(QListBoxItem *item)
			Pokazuje menu dla podanego kontaktu.
			\param item element wy¶wietlanej listy kontaktów reprezentuj±cy rz±dany kontakt.
		**/
		void show(QListBoxItem *item);

		/**
			\fn int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj±cy sygna³ wybrania elementu z menu.
			\param member SLOT obiektu \a receiver który zostanie wykonany po wybraniu elementu z menu.
			\param accel skrót klawiaturowy dla tego elementu. Domy¶lnie brak.
			\param id Unikatowa liczba identyfikuj±ca nowy element. Domy¶lnie pierwsza wolna.
		**/
		int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param iconname nazwa ikony z zestawu lub sciezka do pliku
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj±cy sygna³ wybrania elementu z menu.
			\param member SLOT obiektu \a receiver który zostanie wykonany po wybraniu elementu z menu.
			\param accel skrót klawiaturowy dla tego elementu. Domy¶lnie brak.
			\param id Unikatowa liczba identyfikuj±ca nowy element. Domy¶lnie pierwsza wolna.
		**/
		int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn int addItemAtPos(int index, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param index pozycja (licz±c od 0) na której znale¼æ ma siê nowy element.
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj±cy sygna³ wybrania elementu z menu.
			\param member SLOT obiektu \a receiver który zostanie wykonany po wybraniu elementu z menu.
			\param accel skrót klawiaturowy dla tego elementu. Domy¶lnie brak.
			\param id Unikatowa liczba identyfikuj±ca nowy element. Domy¶lnie pierwsza wolna.
		**/
		int addItemAtPos(int index, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn int addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param index pozycja (licz±c od 0) na której znale¼æ ma siê nowy element.
			\param iconname nazwa ikony z zestawu lub sciezka do pliku
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj±cy sygna³ wybrania elementu z menu.
			\param member SLOT obiektu \a receiver który zostanie wykonany po wybraniu elementu z menu.
			\param accel skrót klawiaturowy dla tego elementu. Domy¶lnie brak.
			\param id Unikatowa liczba identyfikuj±ca nowy element. Domy¶lnie pierwsza wolna.
		**/
		int addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn void refreshIcons()
			Prze³adowuje wszystkie ikony w tym menu dodane przy pomocy powy¿szych funkcji
		**/
		void refreshIcons();
	signals:
		/**
			\fn void popup()
			Sygna³ emitowany, gdy menu jest wywo³ywane.
		**/
		void popup();
};


/**
	Klasa reprezentuj±ca listê kontaktów wraz z ikonkami stanów.
	\class UserBox
	\brief Wy¶wietlana lista kontaktów.
**/
class UserBox : public QListBox, ConfigurationAwareObject
{
	Q_OBJECT

	friend class Kadu;

	public:
		/**
			\fn UserBox(UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0)
			Standardowy konstruktor tworz±cy listê kontaktów.
			\param group grupa kontaktów, która bêdzie wy¶wietlana
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
			\param f flagi kontrolki. Domy¶lnie 0.
		**/
		UserBox(bool fancy, UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0);

		virtual ~UserBox();

		/**
			\var static UserBoxMenu *userboxmenu
			Wska¼nik do menu kontekstowego listy kontaktów.
		**/
		static UserBoxMenu *userboxmenu;

		/**
			\var static UserBoxMenu *management
			Wska¼nik do menu zarz±dzania kontaktami.
		**/
		static UserBoxMenu *management;

		static void setColorsOrBackgrounds();

		/**
			\fn static void initModule()
			Inicjalizuje zmienne niezbêdne do dzia³ania UserBox. Funkcja ta jest
			wywo³ywana przy starcie Kadu przez rdzeñ Kadu.
		**/
		static void initModule();

		static void closeModule();

		/**
			\fn UserListElements getSelectedUsers() const
			Funkcja zwraca listê zaznaczonych uzytkowników.
			\return UserListElements z zaznaczonymi u¿ytkownikami.
		**/
		UserListElements selectedUsers() const;

		/**
			\fn const UserGroup *visibleUsers() const
			zwraca listê kontaktów, które s± obecnie na li¶cie
			\return grupa kontaktów
		**/
		const UserGroup *visibleUsers() const;

		/**
			\fn QValueList<UserGroup *> filters() const
			Zwraca listê filtrów "pozytywnych"
			\return lista filtrów
		**/
		QValueList<UserGroup *> filters() const;

		/**
			\fn QValueList<UserGroup *> negativeFilters() const
			Zwraca listê filtrów "negatywnych"
			\return lista filtrów
		**/
		QValueList<UserGroup *> negativeFilters() const;

		/**
			\fn bool currentUserExists() const
			\return informacja o istnieniu bie¿±cego kontaktu
		**/
		bool currentUserExists() const;

		/**
			\fn UserListElement currentUser() const
			\attention {przed wywo³aniem tej metody nale¿y sprawdziæ, czy istnieje bie¿±cy kontakt!}
			\return bie¿±cy kontakt
			\see UserBox::currentUserExists()
		**/
		UserListElement currentUser() const;

		/**
			\fn static UserBox* activeUserBox()
			Funkcja znajdujaca aktywny UserBox.
			\return wska¼nik do aktywnego UserBox'a, je¶li taki nie istnieje zwracana jest warto¶æ NULL.
		**/
		static UserBox* activeUserBox();

		class CmpFuncDesc
		{
			public:
 				CmpFuncDesc(QString i, QString d, int (*f)(const UserListElement &, const UserListElement &))
 					: id(i), description(d), func(f) {}
 				CmpFuncDesc() : id(), description(), func(0){}

				QString id;
				QString description;
				int (*func)(const UserListElement &, const UserListElement &);
		};

		/**
			\fn QValueList<UserBox::CmpFuncDesc> compareFunctions() const
			\return lista obiektów opisuj±cych funkcje porównuj±ce
		**/
 		QValueList<UserBox::CmpFuncDesc> compareFunctions() const;

		/**
			\fn void addCompareFunction(const QString &id, const QString &trDescription, int (*cmp)(const UserListElement &, const UserListElement &))
			Dodaje funkcjê porównuj±c± do bie¿±cego UserBoksa.
			Funkcja cmp powinna zwracaæ warto¶æ:
				< 0 gdy pierwszy kontakt powinien znale¼æ siê przed drugim
				= 0 gdy kolejno¶æ kontaktów nie ma znaczenia
				> 0 gdy pierwszy kontakt powinien znale¼æ siê za drugim
			\param id krótki identyfikator funkcji
			\param trDescription pzet³umaczony opis funkcji (w zamierzeniu do wy¶wietlenia w konfiguracji)
			\param cmp funkcja porównuj±ca
		**/
		void addCompareFunction(const QString &id, const QString &trDescription,
					int (*cmp)(const UserListElement &, const UserListElement &));

		/**
			\fn static void refreshAll()
			Od¶wie¿a wszystkie UserBoksy.
		**/
		static void refreshAll();

		/**
			\fn static void refreshAllLater()
			Od¶wie¿a wszystkie UserBoksy, ale dopiero po powrocie do pêtli zdarzeñ Qt.
		**/
		static void refreshAllLater();

		static const QValueList<UserBox *> &userboxes() {return UserBoxes;}

		static CreateNotifier createNotifier;

	protected:
		virtual void configurationUpdated();

	public slots:

		/**
			\fn void descriptionsActionActivated(const UserGroup* users, const QWidget* widget, bool toggle)
			Slot jest wywo³ywany, gdy aktywowano ukrywanie opisów kontaktów
			\param users u¿ytkownicy (nieu¿ywany)
			\param widget (nieu¿ywany)
			\param toggle w³±czenie / wy³±czenie ukrywania opisów
		**/
		void descriptionsActionActivated(const UserGroup* users, const QWidget* widget, bool toggle);

		/**
			\fn void setDescriptionsActionState()
			Slot jest wywo³ywany, w celu ustawienia poprawnego stanu dla akcji
			w³±czaj±cej / wy³±czaj±cej pokazywanie opisów na li¶cie kontaktów
		**/
		void setDescriptionsActionState();

		/**
			\fn void applyFilter(UserGroup *group)
			Nak³ada na bie¿±cy UserBox filtr "pozytywny" - wy¶wietlane bêd±
			tylko te kontakty, które nale¿± do group.
			\param group filtr
		**/
		void applyFilter(UserGroup *group);

		/**
			\fn void removeFilter(UserGroup *group)
			Usuwa z bie¿±cego UserBoksa wskazany filtr "pozytywny".
			\param group filtr
		**/
		void removeFilter(UserGroup *group);

		/**
			\fn void applyNegativeFilter(UserGroup *group)
			Nak³ada na bie¿±cy UserBox filtr "negatywny" - wy¶wietlane bêd±
			tylko te kontakty, które nie nale¿± do group.
			\param group filtr
		**/
		void applyNegativeFilter(UserGroup *group);

		/**
			\fn void removeNegativeFilter(UserGroup *group)
			Usuwa z bie¿±cego UserBoksa wskazany filtr "negatywny".
			\param group filtr
		**/
		void removeNegativeFilter(UserGroup *group);

		/**
			\fn void removeCompareFunction(const QString &id)
			Usuwa funkcjê porównuj±c± o identyfikatorze id.
			\param id identyfikator funkcji porównuj±cej
		**/
		void removeCompareFunction(const QString &id);

		/**
			\fn void moveUpCompareFunction(const QString &id)
			Przesuwa funkcjê porównuj±c± o identyfikatorze id wy¿ej w kolejno¶ci sprawdzania.
			\param id identyfikator funkcji porównuj±cej
		**/
		bool moveUpCompareFunction(const QString &id);

		/**
			\fn void moveDownCompareFunction(const QString &id)
			Przesuwa funkcjê porównuj±c± o identyfikatorze id ni¿ej w kolejno¶ci sprawdzania.
			\param id identyfikator funkcji porównuj±cej
		**/
 		bool moveDownCompareFunction(const QString &id);

		/**
			\fn void refresh()
			Od¶wie¿a bie¿±cy UserBox. Nie nale¿y tej funkcji nadu¿ywaæ,
			bo wiêkszo¶æ (90%) sytuacji jest wykrywanych przez sam± klasê.
		**/
		void refresh();

		void refreshLater();
	signals:
		/**
			\fn void doubleClicked(UserListElement user)
			Sygna³ jest emitowany, gdy na którym¶ z kontaktów klikniêto dwukrotnie.
			\param user kontakt, na którym kilkniêto dwukrotnie
			\warning U¿ywaj tego sygna³u zamiast QListBox::doubleClicked(QListBoxItem *) !!!
			Tamten ze wzglêdu na od¶wie¿anie listy w jednym ze slotów pod³±czonych
			do tego sygna³u czasami przekazuje wska¼nik do elementu, który ju¿ NIE ISTNIEJE.
		**/
		void doubleClicked(UserListElement user);

		/**
			\fn void returnPressed(UserListElement user)
			Sygna³ jest emitowany, gdy wci¶niêto klawisz enter na wybranym kontakcie.
			\param user kontakt, na którym wci¶niêto enter
			\warning U¿ywaj tego sygna³u zamiast QListBox::returnPressed(QListBoxItem *) !!!
			Tamten ze wzglêdu na od¶wie¿anie listy w jednym ze slotów pod³±czonych
			do tego sygna³u czasami przekazuje wska¼nik do elementu, który ju¿ NIE ISTNIEJE.
		**/
		void returnPressed(UserListElement user);

		/**
			\fn void currentChanged(UserListElement user)
			Sygna³ jest emitowany, gdy zmieni³ siê bie¿±cy kontakt.
			\attention {raczej nale¿u u¿ywaæ tego sygna³u zamiast QListBox::currentChaned(QListBoxItem *)}
			\param user obecnie bie¿±cy kontakt
		**/
		void currentChanged(UserListElement user);

	private slots:
		void doubleClickedSlot(QListBoxItem *item);
		void returnPressedSlot(QListBoxItem *item);
		void currentChangedSlot(QListBoxItem *item);

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

	private:
		static QValueList<UserBox*> UserBoxes;
		bool fancy;

		Action *desc_action;

		UserGroup *VisibleUsers;
		QValueList<UserGroup *> Filters;
		QValueList<UserGroup *> NegativeFilters;
		std::vector<UserListElement> sortHelper;
		std::vector<UserListElement> toRemove;
		QMap<const UserGroup *, UserListElements> AppendProxy;
		QMap<const UserGroup *, UserListElements> RemoveProxy;

		ULEComparer *comparer;
		void sort();
		QTimer refreshTimer;

		UserListElement lastMouseStopUser;
		static UserListElement nullElement;
		QPoint lastMouseStop;
		QTimer tipTimer;

		QTimer verticalPositionTimer;
		int lastVerticalPosition;

		static QImage *backgroundImage;
		void refreshBackground();

	protected:
		virtual void wheelEvent(QWheelEvent *e);
		virtual void enterEvent(QEvent *);
		virtual void leaveEvent(QEvent *);

		/**
			\fn virtual void mousePressEvent(QMouseEvent *e)
			Wci¶niêto który¶ z przycisków myszki na li¶cie kontaktów.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		virtual void mousePressEvent(QMouseEvent *e);

		virtual void mouseReleaseEvent(QMouseEvent *e);

		/**
			\fn virtual void mouseMoveEvent(QMouseEvent* e)
			Poruszono myszk± nad list± kontaktów.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		virtual void mouseMoveEvent(QMouseEvent* e);

		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Wci¶niêto który¶ z klawiszów w aktywnej li¶cie kontaktów.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		virtual void keyPressEvent(QKeyEvent *e);

		/**
			\fn virtual void resizeEvent(QResizeEvent *)
			Lista kontaktów zmieni³a swój rozmiar.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		virtual void resizeEvent(QResizeEvent *);
};

/**
	\fn int compareAltNick(const UserListElement &u1, const UserListElement &u2)
	Funkcja porównuj±ca AltNicki metod± QString::localeAwareCompare()
**/
int compareAltNick(const UserListElement &u1, const UserListElement &u2);

/**
	\fn int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2)
	Funkcja porównuj±ca AltNicki metod± QString::localeAwareCompare(), wcze¶niej robi±ca lower(),
	a wiêc wolniejsza od compareAltNick
**/
int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2);

/**
	\fn int compareStatus(const UserListElement &u1, const UserListElement &u2)
	Funkcja porównuj±ca statusy w protokole Gadu-Gadu. Uwa¿a status "dostêpny" i "zajêty" za równowa¿ne.
**/
int compareStatus(const UserListElement &u1, const UserListElement &u2);

extern ToolTipClassManager *tool_tip_class_manager;

#endif
