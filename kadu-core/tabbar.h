#ifndef KADU_TABBAR_H
#define KADU_TABBAR_H

#include <QTabBar>

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
		KaduTabBar(QWidget *parent = 0);

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

};

#endif
