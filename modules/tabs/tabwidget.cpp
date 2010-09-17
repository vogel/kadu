/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QDrag>

#include "gui/widgets/buddies-list-widget.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "configuration/configuration-file.h"
#include "gui/hot-key.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "activate.h"

#include "tabwidget.h"

TabWidget::TabWidget()
{
	setWindowRole("kadu-tabs");

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
	//przycisk otwarcia nowej karty pokazywany w lewym gornym rogu

	openChatButton = new QToolButton(this);
	openChatButton->setIcon(IconsManager::instance()->iconByPath("kadu_icons/kadu-chat.png"));
	setCornerWidget(openChatButton, Qt::TopLeftCorner);
	connect(openChatButton, SIGNAL(clicked()), SLOT(newChat()));
	openChatButton->setAutoRaise(true);

	//przycisk zamkniecia aktywnej karty znajdujacy sie w prawym gornym rogu
	closeChatButton = new QToolButton(this);
	closeChatButton->setIcon(IconsManager::instance()->iconByPath("kadu_icons/module_tabs-remove.png"));
	setCornerWidget(closeChatButton, Qt::TopRightCorner);
	connect(closeChatButton, SIGNAL(clicked()), SLOT(deleteTab()));
	closeChatButton->setAutoRaise(true);
#ifdef Q_OS_MAC
	/* Dorr: on Mac make the tabs look like the ones from terminal or safari */
	tabbar->setDocumentMode(true);
	setAttribute(Qt::WA_MacBrushedMetal);
	setStyleSheet("QToolButton { background: transparent; }");
#endif
}

void TabWidget::closeChatWidget(ChatWidget *chat)
{
	if (chat)
		delete chat;
}

void TabWidget::closeEvent(QCloseEvent *e)
{
	//w zaleznosci od opcji w konfiguracji zamykamy wszystkie karty, lub tylko aktywna
	if (config_oldStyleClosing)
	{
		QWidget *current = currentWidget();
		delete current;

	}
	else
	{
		//dopoki sa jeszcze karty zamykamy aktywna
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
	Q_UNUSED(k)

	if (handled)
		return;

	handled = true;
	// obsluga skrotow klawiszowych
	if (HotKey::shortCut(e, "ShortCuts", "MoveTabLeft"))
		moveTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "MoveTabRight"))
		moveTabRight();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
		switchTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
		switchTabRight();
	else
		// skrot nie zostal znaleziony i wykonany. Przekazujemy zdarzenie dalej
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
	// Akceptujemu dnd jezeli pochodzi on z UserBox'a lub paska kart
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

	// Jezeli dnd pochodzil z userboxa probujemy dodac nowa karte
	if (dynamic_cast<BuddiesListWidget *>(e->source()) && false)/*UlesDrag::decode(e, ules))*/
	{
		if (tabbar->tabAt(e->pos()) != -1)
		// Jezeli w miejscu upuszczenia jest karta, dodajemy na jej pozycji
			emit openTab(ules, tabbar->tabAt(e->pos()));
		else
		// Jezeli nie na koncu tabbara
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
	// jezeli dwuklik nastapil lewym przyciskiem myszy pokazujemy okno openchatwith
	if (e->button() == Qt::LeftButton)
		newChat();
	kdebugf2();
}

void TabWidget::newChat()
{
	OpenChatWith::instance()->show();
}

void TabWidget::deleteTab()
{
	// zamykamy biezaca karte
	QWidget *current = currentWidget();
	delete current;
}

void TabWidget::tabInserted(int index)
{
	Q_UNUSED(index)

	show();
}

void TabWidget::tabRemoved(int index)
{
	Q_UNUSED(index)

	if (count() == 0)
		hide();
}

void TabWidget::compositingEnabled()
{
	if (config_file.readBoolEntry("Chat", "UseTransparency", false))
	{
		setAutoFillBackground(false);
		setAttribute(Qt::WA_TranslucentBackground, true);
	}
	else
		compositingDisabled();
}

void TabWidget::compositingDisabled()
{
	setAttribute(Qt::WA_TranslucentBackground, false);
	setAttribute(Qt::WA_NoSystemBackground, false);
	setAutoFillBackground(true);
}

void TabWidget::configurationUpdated()
{
	triggerCompositingStateChanged();

	// odswiezenie ikon
	openChatButton->setIcon(IconsManager::instance()->iconByPath("internet-group-chat.png"));
	closeChatButton->setIcon(IconsManager::instance()->iconByPath("kadu_icons/module_tabs-remove.png"));

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
		emit tabCloseRequested(tabAt(e->pos()));
	QTabBar::mouseReleaseEvent(e);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// w celu ulatwienia sobie zadania przekazujemy zdarzenie dalej- tu klasie tabdialog
	emit mouseDoubleClickEventSignal(e);
	kdebugf2();
}
