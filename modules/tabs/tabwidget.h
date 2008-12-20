#ifndef TABS_TABWIDGET_H
#define TABS_TABWIDGET_H

#include <QtGui/QCursor>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>

#include "chat_widget.h"
#include "custom_input.h"
#include "debug.h"

class OpenChatWith;

class TabBar: public QTabBar
{
	Q_OBJECT

	protected:
		/**
		* Metoda wywoływana w momencie kliknięcia na tabbarze
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void mousePressEvent(QMouseEvent* e);

		/**
		* Metoda wywoływana w momencie puszczenia przycisku myszy
		* po kliknięciu
		* Tu odpowiada za zamknięcie karty środokowym przyciskiem myszy
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void mouseReleaseEvent(QMouseEvent* e);

	 	/**
		* Metoda wywoływana w momencie przesunięcia wskaźnika myszy
		* nad tabbarem
		* Tu aktywuje dnd lub ustawia przycisk zamykania karty we 	
		* właściwym miejscu na karcie w zależności od zdarzenia
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void mouseMoveEvent(QMouseEvent* e);

		/**
		* Metoda wywoływana w momencie opuszczenia wskaźnika myszy
		* powierchni tabbara
		* Tu ukrywa przycisk zamykania karty
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void leaveEvent(QEvent* e);

		/**
		* Metoda wywoływana w momencie dwukliku myszy
		* Tu powoduje otwarcie okna openChatWith;
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void mouseDoubleClickEvent(QMouseEvent* e);

		/**
		* Metoda wywoływana w momencie ruchu kółkiem myszy.
		* Tu zmienia aktywnną kartę zgodnie z ruchem rolki myszy
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void wheelEvent(QWheelEvent* e);

		virtual void tabLayoutChange(); 

	signals:
		void contextMenu(int id, const QPoint& pos);
		void deleteTab(int);
		void wheelEventSignal(QWheelEvent* e);
		void mouseDoubleClickEventSignal(QMouseEvent* e);

	private:
		int clickedItem;
		QToolButton *crossButton;
		bool showCloseButton;
		QPoint MouseStart;

	private slots:
		/**
		* Slot zostaje wywołany w momencie naciźnięcia przycisku 
		* zamknij na karcie
		* Tu emituje sygnał void deleteTab(int);
		*/
		void closeTab();

		/**
		* Slot zostaje wywołany w momencie jakiejkolwiek zmiany wyglądu tabbara 
		* Tu uaktualnia pozycję przycisku zamknięcia na karcie 
		*/
		void replaceCross();

	public:
		TabBar(QWidget *parent = 0, char *name = 0);
		/**
		* Metoda wywoływana w momencie zmiany konfiguracji kadu
		* Tu pokazuje lub ukrywa przycisk zamknięcia chata na karcie
		* @param show pokaz lub ukryj przycisk zmknięcia
		*/
		void setShowCloseButton(bool show);
};

class TabWidget: public QTabWidget, public ChatContainer
{
	Q_OBJECT

	public:
		TabWidget();

		/**
		* Zmienia aktualną kartę na tą po lewej stronie
		*/
		void switchTabLeft();

		/**
		* Zmienia aktualną kartę na tą po prawej stronie
		*/
		void switchTabRight();

		void moveTabLeft();
		void moveTabRight();

		/**
		* Metoda wywoływana w momencie zaakceptowania zmian 
		* w oknie konfiguracji kadu. Ustawua opcje modułu zgodnie
		* z konfiguracją
		*/
		void configurationUpdated();

		/**
		* Metoda wywoływana w momencie ignorowania/blokowania kontaktu 
		* Informuje, który chat powinien zostać zamknięty
		* @param chat który powinien zostać zamknięty
		*/
		void closeChatWidget(ChatWidget *chat);
		TabBar* getTabBar() { return tabbar; };

	private:
		TabBar* tabbar;
		OpenChatWith *openChatWithWindow;
		QToolButton *closeChatButton;
		QToolButton *openChatButton;

		/**
		* Zmienna do której zapisujemy geometrię okienka openchatwith
		* aby po jego zamknięciu przywrócić jego właściwą pozycję
		* w kofiguracji
		*/
		QRect openTabWithGeometry;

		/**
		* Zmienna konfiguracyjna
		*/
		bool config_oldStyleClosing;

	private slots:
		void onContextMenu(int id, const QPoint& pos);

		/**
		* Slot zostaje wywołany w celu zmiany pozycji karty.
		* @param from pozycja karty źródłowej.
		* @param to pozycja karty docelowej.
		*/
		void moveTab(int from, int to);

		/**
		* Slot zostaje wywołany w chwili naciśnięcia przycisku
		* zamknięcia na karcie lub użycia środkowego przycisku myszy
		* @param numer karty, która ma zostać zamknięta.
		*/
		void onDeleteTab(int);

		/**
		* Slot zostaje wywołany w chwili naciśnięcia przycisku
		* otwracia nowej karty. 
		* Tu otwiera okienko lub przywraca ja na pierwszy plan
		*/
		void newChat();

		/**
		* Slot zostaje wywołany w celu zmiany pozycji karty.
		* @param from pozycja karty źródłowej.
		* @param to pozycja karty docelowej.
		*/
		void deleteTab();

		/**
		* Slot zostaje wywołany w momencie zamknięcia okna openChatWith
		* Tu przypisuje wskaźnikowi do okna NULL
		*/
		void openChatWithWindowClose();


	protected:
		/**
		* Metoda wywoływana w chwili naciśnięcia x na ?pasku dekoracji?
		* Odpowiada za zamykanie wszystkich kart, lub tylko aktywnej karty
		* w zależności od konfiguracji
		*/
		virtual void closeEvent(QCloseEvent* e);

		/**
		* Metoda wywoływana w chwili przeciągnięcia obiektu na pasek kart
		* metodą dnd
		* akceptuje zdarzenie jeśli pochodziło z userbox'a lub z tabbara
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void dragEnterEvent(QDragEnterEvent* e);

		/**
		* Metoda wywoływana w chwili upuszczenia przeciągniętego obiektu
		* na pasek kart
		* Wywołuje mętodę odpowiedzialną za przeniesienie karty lub otwarcie
		* nowej w zależnośći od zdarzenia
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void dropEvent(QDropEvent* event);

		virtual void windowActivationChange(bool oldActive);

	public slots:
		/**
		* Slot zostaje wywołany w momencie wprowadzenia znaku w polu edycji 
		* okna chat. Odpowiada za skórty klawiaturowe modułu, Jeśli handled
		* zostanie ustawione na true, skrót został przyęjty i wykonany
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		* @param k wskaźnik do pola edycji z którego zostało wywołane zdarzenie.
		*/
		virtual void chatKeyPressed(QKeyEvent* e, CustomInput* k, bool &handled);

		/**
		* Slot zostaje wywołany w momencie ruchu kółkiem myszy.
		* Tu zmienia aktywnną kartę zgodnie z ruchem rolki myszy
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void wheelEventSlot(QWheelEvent* e);

		/**
		* Slot zostaje wywołany w momencie dwukliku myszy
		* Tu powoduje otwarcie okna openChatWith;
		* @param e wskaźnik obiektu opisującego to zdarzenie.
		*/
		virtual void mouseDoubleClickEvent(QMouseEvent* e);

	signals:
		void contextMenu(QWidget* w, const QPoint& pos);
		void openTab(QStringList altnicks, int index);
		void chatWidgetActivated(ChatWidget *);
};
#endif
