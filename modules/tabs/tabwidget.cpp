#include <QtGui/QDrag>

#include "gui/widgets/contacts-list-widget.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "activate.h"

#include "configuration/configuration-file.h"
#include "gui/hot-key.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "tabwidget.h"

TabWidget::TabWidget()
{
	tabbar = new TabBar(this);
	tabbar->setMovable(true);

	setTabBar(tabbar);
	//akceptujemy dnd
	setAcceptDrops(true);
	connect(tabbar, SIGNAL(contextMenu(int, const QPoint&)),
			SLOT(onContextMenu(int, const QPoint&)));
	connect(tabbar, SIGNAL(tabCloseRequested(int)),
			SLOT(onDeleteTab(int)));
	connect(tabbar,SIGNAL(mouseDoubleClickEventSignal(QMouseEvent*)),
			SLOT(mouseDoubleClickEvent(QMouseEvent*)));
	//przycisk otwarcia nowej karty pokazywany w lewym górnym rogu

	openChatButton = new QToolButton(this);
   	openChatButton->setIcon(IconsManager::instance()->loadIcon("OpenChat"));
	setCornerWidget(openChatButton, Qt::TopLeftCorner);
	connect(openChatButton, SIGNAL(clicked()), SLOT(newChat()));
	openChatButton->setAutoRaise(true);

	//przycisk zamknięcia aktywnej karty znajdujący się w prawym górnym rogu
	closeChatButton = new QToolButton(this);
   	closeChatButton->setIcon(IconsManager::instance()->loadIcon("TabsRemove"));
	setCornerWidget(closeChatButton, Qt::TopRightCorner);
	connect(closeChatButton, SIGNAL(clicked()), SLOT(deleteTab()));
	closeChatButton->setAutoRaise(true);
	openChatWithWindow = NULL;
}

void TabWidget::closeChatWidget(ChatWidget *chat)
{
	if (chat)
		delete chat;
}

void TabWidget::closeEvent(QCloseEvent *e)
{
	//w zależności od opcji w konfiguracji zamykamy wszystkie karty, lub tylko aktywną
	if (config_oldStyleClosing)
	{
		QWidget *current = currentWidget();
		delete current;

	}
	else
	{
		//dopóki są jeszcze karty zamykamy aktywną
		while(count())
		{
			QWidget* current = currentWidget();
			delete current;
		}
	}

	if (count() > 0)
		e->ignore();
	else
		e->accept();
}

void TabWidget::chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled)
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

void TabWidget::onContextMenu(int id, const QPoint &pos)
{
	emit contextMenu(widget(id), pos);
}

void TabWidget::moveTab(int from, int to)
{
	kdebugf();
	QString tablabel=tabText(from);
	QWidget *w = widget(from);
	QIcon tabiconset = tabIcon(from);
	QString tabtooltip = tabToolTip(from);
	bool current = (w == currentWidget());
	blockSignals(true);
	removeTab(from);

	insertTab(to, w, tabiconset, tablabel);
	setTabToolTip(to, tabtooltip);

	if (current)
		setCurrentIndex(to);

	blockSignals(false);
}

void TabWidget::onDeleteTab(int id)
{
	QWidget *chat = widget(id);
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
	if (currentIndex() == (count() - 1))
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
// 	if ((UlesDrag::canDecode(e) && (dynamic_cast<ContactsListWidget *>(e->source()))))
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

	kdebugf2();
}

void TabWidget::windowActivationChange(bool oldActive)
{
	kdebugf();
	ChatWidget *chat = dynamic_cast<ChatWidget*>(currentWidget());
	if (_isActiveWindow(this) && !oldActive && chat)
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
	if (e->button() == Qt::LeftButton)
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
	QWidget *current = currentWidget();
	delete current;
}

void TabWidget::tabInserted(int index)
{
	show();
}

void TabWidget::tabRemoved(int index)
{
	if (count() == 0)
		hide();
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
   	openChatButton->setIcon(IconsManager::instance()->loadIcon("OpenChat"));
   	closeChatButton->setIcon(IconsManager::instance()->loadIcon("TabsRemove"));

	tabbar->setTabsClosable(config_file.readBoolEntry("Tabs", "CloseButtonOnTab"));

	// uaktualniamy zmienne konfiguracyjne
	closeChatButton->setShown(config_file.readBoolEntry("Tabs", "CloseButton"));
	openChatButton->setShown(config_file.readBoolEntry("Tabs", "OpenChatButton"));
	config_oldStyleClosing = config_file.readBoolEntry("Tabs", "OldStyleClosing");
}

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
}

void TabBar::mousePressEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::RightButton)
		emit contextMenu(tabAt(e->pos()), mapToGlobal(e->pos()));

	QTabBar::mousePressEvent(e);
}

void TabBar::mouseReleaseEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::MidButton)
		emit deleteTab(tabAt(e->pos()));
	QTabBar::mouseReleaseEvent(e);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// w celu ułatwienia sobie zadania przekazujemy zdarzenie dalej- tu klasie tabdialog
	emit mouseDoubleClickEventSignal(e);
	kdebugf2();
}
