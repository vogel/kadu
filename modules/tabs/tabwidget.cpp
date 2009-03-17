#include <QtGui/QDrag>

#include "gui/widgets/contacts-list-widget.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "config_file.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "misc.h"

#include "tabwidget.h"

TabWidget::TabWidget()
{
	tabbar = new TabBar(this);
	setTabBar(tabbar);
	//akceptujemy dnd
	setAcceptDrops(true);
	connect(tabbar, SIGNAL(contextMenu(int, const QPoint&)),
			SLOT(onContextMenu(int, const QPoint&)));
	connect(tabbar, SIGNAL(deleteTab(int)),
			SLOT(onDeleteTab(int)));
	connect(tabbar, SIGNAL(wheelEventSignal(QWheelEvent*)),
			SLOT(wheelEventSlot(QWheelEvent*)));
	connect(tabbar,SIGNAL(mouseDoubleClickEventSignal(QMouseEvent*)),
			SLOT(mouseDoubleClickEvent(QMouseEvent*)));
	//przycisk otwarcia nowej karty pokazywany w lewym górnym rogu
	openChatButton = new QToolButton(this);
   	openChatButton->setIcon(icons_manager->loadIcon("OpenChat"));
	setCornerWidget(openChatButton, Qt::TopLeftCorner);
	connect(openChatButton, SIGNAL(clicked()),SLOT(newChat()));
	openChatButton->setAutoRaise(true);
	//przycisk zamknięcia aktywnej karty znajdujący się w prawym górnym rogu
	closeChatButton = new QToolButton(this);
   	closeChatButton->setIcon(icons_manager->loadIcon("TabsRemove"));
	setCornerWidget(closeChatButton, Qt::TopRightCorner);
	connect(closeChatButton, SIGNAL(clicked()),SLOT(deleteTab()));
	closeChatButton->setAutoRaise(true);
	openChatWithWindow=NULL;
}

void TabWidget::closeChatWidget(ChatWidget *chat)
{
	if (chat)
		delete chat;
}

void TabWidget::closeEvent(QCloseEvent* e)
{
	//w zależności od opcji w konfiguracji zamykamy wszystkie karty, lub tylko aktywną
	if(config_oldStyleClosing)
	{
		QWidget* current = currentWidget();
		delete current;

	}
	else
		//dopóki są jeszcze karty zamykamy aktywną
		while(count())
		{
			QWidget* current = currentWidget();
			delete current;
		}
	if (count() > 0)
		e->ignore();
	else
		e->accept();
}

void TabWidget::chatKeyPressed(QKeyEvent* e, CustomInput* k, bool &handled)
{
	handled = true;
	// obsługa skrótów klawiszowych
	if (HotKey::shortCut(e, "ShortCuts", "MoveTabLeft"))
		moveTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "MoveTabRight"))
		moveTabRight();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
		switchTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
		switchTabRight();
	else
		// skrót nie został znaleziony i wykonany. Przekazujemy zdarzenie dalej
		handled = false;
}

void TabWidget::onContextMenu(int id, const QPoint& pos)
{
	emit contextMenu(widget(id), pos);
}

void TabWidget::wheelEventSlot(QWheelEvent* e)
{
	if (e->delta() > 0)
		switchTabLeft();
	else
		switchTabRight();
}

void TabWidget::moveTab(int from, int to)
{
	kdebugf();
	QString tablabel=tabText(from);
	QWidget *w=widget(from);
	QIcon tabiconset = tabIcon(from);
	QString tabtooltip = tabToolTip(from);
	bool current=(w==currentWidget());
	blockSignals(true);
	removeTab(from);

	insertTab(to, w, tabiconset, tablabel);
	setTabToolTip(to, tabtooltip);

	if(current)
		setCurrentIndex(to);

	blockSignals(false);
}

void TabWidget::onDeleteTab(int id)
{
	QWidget* chat = widget(id);
	delete chat;
}

void TabWidget::switchTabLeft()
{
	if (currentIndex() == 0)
		setCurrentIndex(count() - 1);
	else
		setCurrentIndex(currentIndex()-1);
}

void TabWidget::switchTabRight()
{
	if (currentIndex()==(count()-1))
		setCurrentIndex(0);
	else
		setCurrentIndex(currentIndex()+1);
}

void TabWidget::moveTabLeft()
{
	if (currentIndex() == 0)
		moveTab(0, count() - 1);
	else
		moveTab(currentIndex(), currentIndex() - 1);
}

void TabWidget::moveTabRight()
{
	if (currentIndex() == (count() - 1))
		moveTab(count() - 1, 0);
	else
		moveTab(currentIndex(), currentIndex() + 1);
}

void TabWidget::dragEnterEvent(QDragEnterEvent* e)
{
	kdebugf();
	// Akceptujemu dnd jeśli pochodzi on z UserBox'a lub paska kart
// 	if ((UlesDrag::canDecode(e) && (dynamic_cast<ContactsListWidget *>(e->source()))) || (e->mimeData()->hasText() && dynamic_cast<TabBar*>(e->source())))
// 		e->acceptProposedAction();
// 	else
		e->ignore();
//
	kdebugf2();
}

void TabWidget::dropEvent(QDropEvent* e)
{
	kdebugf();
	QStringList ules;

	// Jeśli dnd pochodził z userboxa próbujemy dodać nową kartę
	if (dynamic_cast<ContactsListWidget *>(e->source()) && false)/*UlesDrag::decode(e, ules))*/
	{
		if (tabbar->tabAt(e->pos()) != -1)
		// Jeśli w miejscu upuszczenia jest karta, dodajemy na jej pozycji
			emit openTab(ules, tabbar->tabAt(e->pos()));
		else
		// Jeśli nie na końcu tabbara
			emit openTab(ules, -1);
	}
	// Jeśli dnd pochodził z tabbara, zmieniamy pozycję kart
	else if(dynamic_cast<TabBar*>(e->source()) && e->mimeData()->hasText())
	{
		QString altnicks = e->mimeData()->text();
		// karta źródłowa
		int movingTabId = altnicks.toUInt();
		// karta docelowa
		int destinationTabId;
		if (tabbar->tabAt(e->pos()) != -1)
		// jeśli w miejscu upuszczenia jest inna karta staje się ona kartą docelową
			destinationTabId = tabbar->tabAt(e->pos());
		else
		// Jeśli nie to kartą docelową jest ostatnia karta
			destinationTabId = count() - 1;
		if(movingTabId != -1 && destinationTabId != movingTabId)
                         // zamieniamy miejcami kartę źródłową z docelową
			moveTab(movingTabId, destinationTabId);
	}

	kdebugf2();
}

void TabWidget::windowActivationChange(bool oldActive)
{
	kdebugf();
	ChatWidget *chat = dynamic_cast<ChatWidget*>(currentWidget());
	if (isActiveWindow() && !oldActive && chat)
	{
		chat->markAllMessagesRead();
		emit chatWidgetActivated(chat);
	}
	kdebugf2();
}

void TabWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// jeśli dwuklik nastąpil lewym przyciskiem myszy pokazujemy okno openchatwith
	if(e->button() == Qt::LeftButton)
		newChat();
	kdebugf2();
}

void TabWidget::newChat()
{
	// jeśli okno openchatwith nie istnieje tworzymy nowe
	if (!openChatWithWindow)
	{
		openChatWithWindow = new OpenChatWith;
   		connect(openChatWithWindow, SIGNAL(destroyed()), this, SLOT(openChatWithWindowClose()));
		// zapisujemy geometrię okna, aby ją później przywrócić (tabsy nie będą psuły pozycji okna z akcji kadu)
		openTabWithGeometry = openChatWithWindow->frameGeometry();
		// pokazujemy je w miejscu w którym nastąpiło wywołanie metody (obecnie znajduje się kursor)
		openChatWithWindow->setGeometry(QCursor::pos().x(), QCursor::pos().y(), openChatWithWindow->width(), openChatWithWindow->height());// jak wykonuje się to po pokazaniu okienkoa występuje denerwujące miganie
		openChatWithWindow->show();
	}
	// jeśli istnieje przywracamy je na pierwszy plan w miejscu cursora
	else
	{
		openChatWithWindow->setGeometry(QCursor::pos().x(), QCursor::pos().y(), openChatWithWindow->width(), openChatWithWindow->height());// jak wykonuje się to po pokazaniu okienka występuje denerwujące miganie
		openChatWithWindow->setWindowState(openChatWithWindow->windowState() & Qt::WindowMinimized);
		openChatWithWindow->raise();
	}
}

void TabWidget::deleteTab()
{
	// zamykamy bieżącą kartę
	QWidget* current = currentWidget();
	delete current;
}

void TabWidget::openChatWithWindowClose()
{
	// ponownie zapisujemy poprawną pozycję okna openTabWith
	config_file.writeEntry("General", "OpenChatWith", openTabWithGeometry);
	// po zamknięciu okna openchatwith przypisujemy NULL do wskaźnika do niego,
	openChatWithWindow = 0;
}

void TabWidget::configurationUpdated()
{
	// odświeżenie ikon
   	openChatButton->setIcon(icons_manager->loadIcon("OpenChat"));
   	closeChatButton->setIcon(icons_manager->loadIcon("TabsRemove"));

	// uaktualniamy zmienne konfiguracyjne
	closeChatButton->setShown(config_file.readBoolEntry("Tabs", "CloseButton"));
	openChatButton->setShown(config_file.readBoolEntry("Tabs", "OpenChatButton"));
	config_oldStyleClosing = config_file.readBoolEntry("Tabs", "OldStyleClosing");
	tabbar->setShowCloseButton(config_file.readBoolEntry("Tabs", "CloseButtonOnTab"));
}

TabBar::TabBar(QWidget *parent, char *name)
	: QTabBar(parent)
{
	// dzięki temu sygnał mouseMoveEvent jest emitowany także w momencie "zwykłego" poruszania się kursora myszy nad tabbarem
	setMouseTracking(true);

	// sposób działania przycisku na każdej karcie bazuje na rozwiazaniu z QDevelop (http://qdevelop.org)
	// Tworzony jest przycisk zamknięcia karty pokazywany na aktywnej karcie
	crossButton = new QToolButton(this);
   	crossButton->hide();
   	connect(crossButton, SIGNAL(clicked()), this, SLOT(closeTab()) );
   	crossButton->setGeometry(0,0,15,15);
   	crossButton->setIcon(icons_manager->loadIcon("TabsClose"));
	// staje się "podświetlany"
	crossButton->setAutoRaise(true);
	// domyśnie żadna karta nie jest "naciśnięta" (nie znajduje się na niej przycisk zamknięcia)
	clickedItem=-1;
}

void TabBar::mousePressEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::RightButton)
		emit contextMenu(tabAt(e->pos()), mapToGlobal(e->pos()));
	MouseStart = e->pos();

	QTabBar::mousePressEvent(e);
}

void TabBar::mouseReleaseEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::MidButton)
		emit deleteTab(tabAt(e->pos()));
	QTabBar::mouseReleaseEvent(e);
}

void TabBar::wheelEvent(QWheelEvent *e)
{
	emit wheelEventSignal(e);
}

void TabBar::mouseMoveEvent(QMouseEvent* e)
{
	kdebugf();
	// Jeśli zaznaczona jest karta i wciśnięty lewy przycisk myszy, inicjujemy dnd
	if ((e->buttons() & Qt::LeftButton) && (tabAt(MouseStart) != -1) && ((MouseStart - e->pos()).manhattanLength() >= 15))
	{
		// "przenoszonym" tekstem jest numer aktywowanej karty
		QString drag_text = QString::number(tabAt(MouseStart));
		// Bezpośrednia inicjacja dnd
		QDrag* drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;

		mimeData->setText(drag_text);
		drag->setMimeData(mimeData);

		drag->exec(Qt::CopyAction);
	}

	// pomysł rozwiazania zaczerpnięty z QDevelop
	// Jeśli aktywna jest opcja pokazywania przycisku zamknięcia na kartach i w obrębie zdarzenia jest karta
	else if (tabAt(e->pos()) != -1 && showCloseButton)
	{
		// pokazujemy przycisk na odpowiedniej pozycji
		clickedItem = tabAt(e->pos());
		// pierwszy parametr ustawia przycisk 5 pixeli od prawej krawędzi karty.
		//TODO: FIX this!
		crossButton->setGeometry(tabRect(clickedItem).x()+tabRect(clickedItem).width()-crossButton->width()-5, 6, crossButton->width(), crossButton->height());
                crossButton->show();
	}
	else

		QTabBar::mouseMoveEvent(e);
	kdebugf2();
}

void TabBar::leaveEvent(QEvent* e)
{
	// w chwili gdy kursor opuszcza tabbar chowamy przycisk zamknięcia
       	crossButton->hide();
}

void TabBar::closeTab()
{
	emit deleteTab(clickedItem);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// w celu ułatwienia sobie zadania przekazujemy zdarzenie dalej- tu klasie tabdialog
	emit mouseDoubleClickEventSignal(e);
	kdebugf2();
}

void TabBar::setShowCloseButton(bool show)
{
	// odświeżenie ikonki
   	crossButton->setIcon(icons_manager->loadPixmap("TabsClose"));

	showCloseButton = show;
	// w zaleźności czy w konfiguracji włączone jest pokazywanie przycisku zamykania na kartach ukrywamy go lub pokazujemy
	if(!showCloseButton)
		crossButton->hide();
}

void TabBar::tabLayoutChange()
{
	QTabBar::tabLayoutChange();
	replaceCross();
}

void TabBar::replaceCross()
{
	// jeśli przycisk zamknięciakarty jest widoczny na karcie i w miejscu kursora znajuje się karta
	if(crossButton->isVisible() && tabAt(mapFromGlobal(QCursor::pos())) != -1)
	{
		// uaktualniamy pozycję przycisku w prawym rogu karty
		clickedItem = tabAt(mapFromGlobal(QCursor::pos()));
		// pierwszy parametr ustawia przycisk 5 pixeli od prawej krawędzi karty.
		crossButton->setGeometry(tabRect(clickedItem).x()+tabRect(clickedItem).width()-crossButton->width()-5, 6, crossButton->width(), crossButton->height());
                crossButton->show();
	}

}
