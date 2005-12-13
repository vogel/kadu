#ifndef KADU_USERBOX_H
#define KADU_USERBOX_H

#include <qlistbox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qtooltip.h>

#include <vector>

#include "gadu.h"
#include "userlistelement.h"

class QFontMetrics;
class UserBoxSlots;
class ULEComparer;

/**
	Klasa reprezentuj±ca kontakt wizualnie na li¶cie kontaktów. Opisuje ona ikonê kontaktu,
	jego wy¶witlan± nazwê oraz ewentualny opis.
	\class KaduListBoxPixmap
	\brief Klasa reprezentuj±ca kontakt wizualnie.
**/
class KaduListBoxPixmap : public QListBoxItem
{
	public:
		/**
			\fn KaduListBoxPixmap(const QPixmap &pix, UserListElement user, bool bold = false)
			Konstruktor dodaj±cy kontakt do listy z podan± ikon±.
			\param pix ikona wy¶wietlana obok kontaktu.
			\param user kontakt, z którego bêd± pobierane dane do wy¶wietlenia
			\param bold warto¶æ logiczna informuj±ca o tym, czy nazwa kontaktu
				ma byæ wy¶wietlana pogrubion± czcionk±.
		**/
		KaduListBoxPixmap(const QPixmap &pix, UserListElement user, bool bold = false);

		/**
			\fn bool isBold() const
			Zwraca informacjê, czy nazwa kontaktu jest wy¶wietlana pogrubion± czcionk±, czy nie.
		**/
		bool isBold() const { return bold; }

		/**
			\fn int height(const QListBox *lb) const
			Zwraca wysoko¶æ elementu reprezentuj±cego wy¶wietlany kontakt w pikselach.
			\param lb obiekt reprezentuj±cy wy¶wietlany kontakt.
		**/
		int height(const QListBox *lb) const;

		/**
			\fn int width(const QListBox *lb) const
			Zwraca szeroko¶æ elementu reprezentuj±cego wy¶wietlany kontakt w pikselach.
			\param lb obiekt reprezentuj±cy wy¶wietlany kontakt.
		**/
		int width(const QListBox *lb) const;

		const UserListElement User;
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
			\fn void setBold(bool b)
			Ustawia stan pogrubienia czcionki wy¶wietlanego kontaktu.
			\param b warto¶æ logiczna okre¶laj±ca, czy czcionka ma byæ pogrubiona czy nie.
		**/
		void setBold(bool b) { bold = b; }

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
		friend class UserBoxSlots;

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
		static void setMultiColumn(bool m);

		/**
			\fn static void setMultiColumnWidth(int w)
			Ustawia szeroko¶æ jednej kolumny dla wy¶wietlania listy w trybie wielu kolumn.
			\param w szeroko¶æ kolumny w pikselach.
		**/
		static void setMultiColumnWidth(int w);

		/**
			\fn static void setDescriptionColor(const QColor &col)
			Ustawia kolor wy¶wietlanego opisu.
			\param col kolor
		**/
		static void setDescriptionColor(const QColor &col);

	private:
		QPixmap pm;
		bool bold;
		static QFontMetrics *descriptionFontMetrics;

		static UinType myUIN;
		static bool ShowDesc;
		static bool AlignUserboxIconsTop;
		static bool ShowMultilineDesc;
		static bool MultiColumn;
		static int  MultiColumnWidth;
		static QColor descColor;

		mutable QString buf_text;
		mutable int buf_width;
		mutable QStringList buf_out;
		mutable int buf_height;
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
class UserBox : public QListBox, private QToolTip
{
	Q_OBJECT

	public:
		/**
			\fn UserBox(UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0)
			Standardowy konstruktor tworz±cy listê kontaktów.
			\param group grupa kontaktów, która bêdzie wy¶wietlana
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
			\param f flagi kontrolki. Domy¶lnie 0.
		**/
		UserBox(UserGroup *group = userlist, QWidget* parent = 0, const char* name = 0, WFlags f = 0);

		~UserBox();

		/**
			\var static UserBoxMenu *userboxmenu
			Wska¼nik do menu kontekstowego listy kontaktów.
		**/
		static UserBoxMenu *userboxmenu;

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
 				CmpFuncDesc(){}

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
			\fn static void refreshAll()
			Od¶wie¿a wszystkie UserBoksy, ale dopiero po powrocie do pêtli zdarzeñ Qt.
		**/
		static void refreshAllLater();
	public slots:

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
		void moveUpCompareFunction(const QString &id);

		/**
			\fn void moveDownCompareFunction(const QString &id)
			Przesuwa funkcjê porównuj±c± o identyfikatorze id ni¿ej w kolejno¶ci sprawdzania.
			\param id identyfikator funkcji porównuj±cej
		**/
 		void moveDownCompareFunction(const QString &id);

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

		void statusChanged(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);
		void protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

	private:
		static QValueList<UserBox*> UserBoxes;
		static UserBoxSlots *userboxslots;

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

		friend class UserBoxSlots;

	protected:
		/**
			\fn virtual void maybeTip(const QPoint& c)
			Wy¶wietla dymek w danym punkcie z informacj± o kontakcie znajduj±cym siê w tym punkcie
			wzglêdem lewego górnego rogu listy kontaktów.
			\param c wspó³rzêdne punktu
		**/
		virtual void maybeTip(const QPoint&);

		/**
			\fn virtual void mousePressEvent(QMouseEvent *e)
			Wci¶niêto który¶ z przycisków myszki na li¶cie kontaktów.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		virtual void mousePressEvent(QMouseEvent *e);

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
	Klasa ta pozwala na podgl±d wprowadzanych zmian konfiguracyjnych wobec UserBox
	w oknie konfiguracji, zanim zaaplikuje siê je do Kadu.
	\class UserBoxSlots
	\brief Obs³uga UserBox w konfiguracji.
**/
class UserBoxSlots : public QObject
{
	Q_OBJECT
	public slots:
		/**
			\fn void onCreateTabLook()
			Obs³uguje sekcjê UserBox podczas otwierania okna konfiguracji.
		**/
		void onCreateTabLook();

		/**
			\fn void onDestroyConfigDialog()
			Obs³uguje sekcjê UserBox podczas zamykania okna konfiguracji.
		**/
		void onApplyTabLook();

		/**
			\fn void chooseColor(const char *name, const QColor& color)
			Od¶wierza podgl±d wybranego koloru.
			\param name nazwa elementu, dla którego wybrano kolor.
				\arg \c userbox_bg_color oznacza kolor t³a.
				\arg \c userbox_font_color oznacz kolor czcionki.
			\param color wybrany kolor.
		**/
		void chooseColor(const char *name, const QColor& color);

		/**
			\fn void chooseFont(const char *name, const QFont& font)
			Od¶wierza podgl±d wybranej czcionki.
			\param name nazwa elementu, dla którego wybrano czcionkê.
				\arg \c userbox_font_box oznacza ogóln± czcionkê listy kontaktów.
			\param font wybrana czcionka.
		**/
		void chooseFont(const char *name, const QFont& font);

		/**
			\fn void onMultiColumnUserbox(bool toggled)
			W³±cza b±d¼ wy³±cza kontrolkê szeroko¶ci kolumny listy kontaktów, w zale¿no¶ci od podanego argumentu.
			\param togglet warto¶æ logiczna informuj±ca, czy kontrolka ma byæ w³±czona czy wy³±czona.
		**/
		void onMultiColumnUserbox(bool toggled);

		/**
			\fn void updatePreview()
			Od¶wierza podgl±d wszystkich elementów UserBox'a.
		**/
		void updatePreview();

		void chooseBackgroundFile();
		void userboxBackgroundMove(bool toggled);
	private slots:
		void backgroundFileChanged(const QString &text);
};

/**
	\fn int compareAltNick(const UserListElement &u1, const UserListElement &u2)
	Funkcja porównuj±ca AltNicki metod± QString::localeAwareCompare()
**/
int compareAltNick(const UserListElement &u1, const UserListElement &u2);

/**
	\fn int compareStatus(const UserListElement &u1, const UserListElement &u2)
	Funkcja porównuj±ca statusy w protokole Gadu-Gadu. Uwa¿a status "dostêpny" i "zajêty" za równowa¿ne.
**/
int compareStatus(const UserListElement &u1, const UserListElement &u2);

#endif
