#ifndef KADU_TABBAR_H
#define KADU_TABBAR_H

#include <qglobal.h>

#include <qtabbar.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <Q3PtrList>

class QPainter;
class QToolButton;

/**
	Klasa ta jest odpowiedzialna za obs³ugê zak³adek grup kontaktów.
	\class KaduTabBar
	\brief Zak³adki grup.
**/
class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		/**
			\fn KaduTabBar(QWidget *parent = 0, const char *name = 0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki.
			\param name nazwa kontrolki.
		**/
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
		~KaduTabBar();

		/**
			\fn void layoutTabs()
			Ustawia zak³adki w odpowiedniej pozycji.
		**/
		void layoutTabs();

		/**
			\fn QSize sizeHint() const
			Sugeruje odpowiedni rozmiar zak³adek.
			\return sugerowany rozmiar.
		**/
		QSize sizeHint() const;

		/**
			\fn QSize minimumSizeHint() const
			Sugeruje minimalny rozmiar zak³adek.
			\return sugerowany rozmiar.
		**/
		QSize minimumSizeHint() const;

		/**
			\fn int insertTab(QTab *newTab, int index = -1)
			Dodaje now± zak³adkê.
			\param newTab wska¼nik do obiektu zak³adki, któr± dodajemy.
			\param index pozycja, w któr± ma byæ wstawiona nowa zak³adka. Domy¶lnie -1 (na koñcu).
		**/
// 		int insertTab(QString *newTab, int index = -1);

		/**
			\fn void removeTab(QTab *t)
			Usuwa zak³adkê.
			\param t wska¼nik do obiektu zak³adki, któr± usuwamy.
		**/
// 		void removeTab(QString *t);

	public slots:

		/**
			\fn void setCurrentTab(QTab *tab)
			Ustawia dan± zak³adkê jako bierz±c±.
			\param tab Wska¼nik do obiektu zak³adki, któr± ustawiamy.
		**/
// 		void setCurrentTab(QString tab);

	private slots:
		void scrollTabsVert();

	protected:
		/**
			\fn void paint(QPainter *p, QTab *t, bool selected) const
			Rysuje dan± zak³adkê.
			\param p urz±dzenie rysuj±ce.
			\param t wska¼nik do obiektu zak³adki, któr± chcemy rysowaæ.
			\param selected informuje o tym, czy zak³adka jest wybran± (bierz±c±),
				czy jedn± z zak³adek zas³oniêtych.
		**/
		void paint(QPainter *p, int index, bool selected) const;

		/**
			\fn void updateArrowButtonsVert()
			Od¶wierza przyciski s³u¿±ce do przewijania listy zak³adek (gdy jest d³uga).
		**/
		void updateArrowButtonsVert();

		/**
			\fn void makeVisibleVert(QTab *t)
			W³±cza przyciski s³u¿±ce do przewijania listy zak³adekna danej zak³adce.
			\param t wska¼nik do obiektu zak³adki, na której w³±czamy przyciski.
			TODO: Nie jestem pewien czy ta metoda dok³adnie to robi.
				Niech to poprawi kto¶, kto t± metodê pisa³ :)
		**/
// 		void makeVisibleVert(QString t);

		/**
			\fn void resizeEvent(QResizeEvent *)
			Metoda obs³uguj±ca zdarzenie zmiany rozmiaru paska zak³adek.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		void resizeEvent(QResizeEvent *e);

		/**
			\fn void dragEnterEvent(QDragEnterEvent* e)
			Metoda obs³uguj±ca zdarzenie "przeci±gniêcia" nad listê zak³adek "podniesionego" wcze¶niej elementu.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		void dragEnterEvent(QDragEnterEvent* e);

		/**
			\fn void dropEvent(QDropEvent* e)
			Metoda obs³uguj±ca zdarzenie "upuszczenia" nad listê zak³adek "podniesionego" wcze¶niej elementu.
			\param e wska¼nik obiektu opisuj±cego to zdarzenie.
		**/
		void dropEvent(QDropEvent* e);

		/**
			\var QPtrList<QTab> *lstatic2
			Lista wska¼ników do obiektów zak³adek.
		**/
		Q3PtrList<QString> *lstatic2;

		/**
			\var bool vertscrolls
			Informuje o tym, czy pasek zak³adek jest ju¿ na tyle d³ugi, ze trzeba go przewijaæ.
		**/
		bool vertscrolls;

		/**
			\var QToolButton *upB
			Wska¼nik do obiektu przycisku przewijaj±cego zak³adki w górê.
		**/
		QToolButton *upB;

		/**
			\var QToolButton *downB
			Wska¼nik do obiektu przycisku przewijaj±cego zak³adki w dó³.
		**/
		QToolButton *downB;
};

#endif
