/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TABS_TABWIDGET_H
#define TABS_TABWIDGET_H

#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>

#include "gui/widgets/chat-widget-container.h"
#include "gui/widgets/custom-input.h"

#include "os/generic/compositing-aware-object.h"

#include "debug.h"

class TabsManager;

class TabBar: public QTabBar
{
	Q_OBJECT

protected:
	/**
	* Metoda wywoływana w momencie kliknięcia na tabbarze
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mousePressEvent(QMouseEvent *e);

	/**
	* Metoda wywoływana w momencie puszczenia przycisku myszy
	* po kliknięciu
	* Tu odpowiada za zamknięcie karty środokowym przyciskiem myszy
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseReleaseEvent(QMouseEvent *e);

	/**
	* Metoda wywoływana w momencie dwukliku myszy
	* Tu powoduje otwarcie okna openChatWith;
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

public:
	explicit TabBar(QWidget *parent = 0);

signals:
	void contextMenu(int id, const QPoint &pos);
	void mouseDoubleClickEventSignal(QMouseEvent *e);

};

class TabWidget: public QTabWidget, public ChatWidgetContainer, CompositingAwareObject
{
	Q_OBJECT

	QToolButton *CloseChatButton;
	QToolButton *OpenChatButton;

	TabsManager *Manager;

	/**
	* Zmienna konfiguracyjna
	*/
	bool config_oldStyleClosing;

private slots:
	void onContextMenu(int id, const QPoint &pos);

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

protected:
	/**
	* Metoda wywoływana w chwili naciśnięcia x na ?pasku dekoracji?
	* Odpowiada za zamykanie wszystkich kart, lub tylko aktywnej karty
	* w zależności od konfiguracji
	*/
	virtual void closeEvent(QCloseEvent *e);

	/**
	* Metoda wywoływana w chwili przeciągnięcia obiektu na pasek kart
	* metodą dnd
	* akceptuje zdarzenie jeśli pochodziło z userbox'a lub z tabbara
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void dragEnterEvent(QDragEnterEvent *e);

	/**
	* Metoda wywoływana w chwili upuszczenia przeciągniętego obiektu
	* na pasek kart
	* Wywołuje mętodę odpowiedzialną za przeniesienie karty lub otwarcie
	* nowej w zależnośći od zdarzenia
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void dropEvent(QDropEvent *event);

	virtual void changeEvent(QEvent *event);

	virtual void tabInserted(int index);
	virtual void tabRemoved(int index);

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	explicit TabWidget(TabsManager *manager);
	virtual ~TabWidget();

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

	virtual void activateChatWidget(ChatWidget *chatWidget);
	virtual void alertChatWidget(ChatWidget *chatWidget);
	virtual bool isChatWidgetActive(ChatWidget *chatWidget);

	/**
	* Metoda wywoływana w momencie ignorowania/blokowania kontaktu
	* Informuje, który chat powinien zostać zamknięty
	* @param chat który powinien zostać zamknięty
	*/
	virtual void closeChatWidget(ChatWidget *chatWidget);

public slots:
	/**
	* Slot zostaje wywołany w momencie wprowadzenia znaku w polu edycji
	* okna chat. Odpowiada za skórty klawiaturowe modułu, Jeśli handled
	* zostanie ustawione na true, skrót został przyęjty i wykonany
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	* @param k wskaźnik do pola edycji z którego zostało wywołane zdarzenie.
	*/
	virtual void chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled);

	/**
	* Slot zostaje wywołany w momencie dwukliku myszy
	* Tu powoduje otwarcie okna openChatWith;
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

signals:
	void contextMenu(QWidget *w, const QPoint &pos);
	void openTab(QStringList altnicks, int index);

};

#endif // TABS_TABWIDGET_H
