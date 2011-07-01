/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "gui/widgets/chat-widget.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "configuration/configuration-file.h"
#include "gui/hot-key.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"

#include "activate.h"

#include "tabwidget.h"

TabWidget::TabWidget()
{
	setWindowRole("kadu-tabs");

	TabBar *tabbar = new TabBar(this);
	setTabBar(tabbar);

	setAcceptDrops(true);
	setMovable(true);

	setDocumentMode(true);

#ifdef Q_OS_MAC
	/* Dorr: on Mac make the tabs look like the ones from terminal or safari */
	setAttribute(Qt::WA_MacBrushedMetal);
	setStyleSheet("QToolButton { background: transparent; }");
#endif

	connect(tabbar, SIGNAL(contextMenu(int, const QPoint &)),
			SLOT(onContextMenu(int, const QPoint &)));
	connect(tabbar, SIGNAL(tabCloseRequested(int)),
			SLOT(onDeleteTab(int)));
	connect(tabbar,SIGNAL(mouseDoubleClickEventSignal(QMouseEvent *)),
			SLOT(mouseDoubleClickEvent(QMouseEvent *)));

	//przycisk otwarcia nowej karty pokazywany w lewym gornym rogu
	OpenChatButton = new QToolButton(this);
	OpenChatButton->setIcon(KaduIcon("kadu_icons/chat").icon());
	OpenChatButton->setAutoRaise(true);
	OpenChatButton->setVisible(false);
	connect(OpenChatButton, SIGNAL(clicked()), SLOT(newChat()));

	//przycisk zamkniecia aktywnej karty znajdujacy sie w prawym gornym rogu
	CloseChatButton = new QToolButton(this);
	CloseChatButton->setIcon(KaduIcon("kadu_icons/tab-remove").icon());
	CloseChatButton->setAutoRaise(true);
	CloseChatButton->setVisible(false);
	connect(CloseChatButton, SIGNAL(clicked()), SLOT(deleteTab()));
}

TabWidget::~TabWidget()
{
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
		delete currentWidget();
	else
		while(count())
			delete currentWidget();

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
	else if (HotKey::shortCut(e, "ShortCuts", "MoveTabRight"))
		moveTabRight();
	else if (HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
		switchTabLeft();
	else if (HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
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
	QString tablabel = tabText(from);
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
	delete widget(id);
}

void TabWidget::switchTabLeft()
{
	if (currentIndex() == 0)
		setCurrentIndex(count() - 1);
	else
		setCurrentIndex(currentIndex() - 1);
}

void TabWidget::switchTabRight()
{
	if (currentIndex() == (count() - 1))
		setCurrentIndex(0);
	else
		setCurrentIndex(currentIndex() + 1);
}

void TabWidget::moveTabLeft()
{
	if (count() == 1)
		return;

	if (currentIndex() == 0)
		moveTab(0, count() - 1);
	else
		moveTab(currentIndex(), currentIndex() - 1);
}

void TabWidget::moveTabRight()
{
	if (count() == 1)
		return;

	if (currentIndex() == (count() - 1))
		moveTab(count() - 1, 0);
	else
		moveTab(currentIndex(), currentIndex() + 1);
}

void TabWidget::dragEnterEvent(QDragEnterEvent* e)
{
	kdebugf();
	// Akceptujemu dnd jezeli pochodzi on z UserBox'a lub paska kart
// 	if ((UlesDrag::canDecode(e) && (qobject_cast<ContactsListWidget *>(e->source()))))
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
	if (qobject_cast<BuddiesListWidget *>(e->source()) && false)/*UlesDrag::decode(e, ules))*/
	{
		if (tabBar()->tabAt(e->pos()) != -1)
		// Jezeli w miejscu upuszczenia jest karta, dodajemy na jej pozycji
			emit openTab(ules, tabBar()->tabAt(e->pos()));
		else
		// Jezeli nie na koncu tabbara
			emit openTab(ules, -1);
	}

	kdebugf2();
}

void TabWidget::changeEvent(QEvent *event)
{
	QTabWidget::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		kdebugf();
		ChatWidget *chat = static_cast<ChatWidget *>(currentWidget());
		if (chat && _isActiveWindow(this))
		{
			chat->markAllMessagesRead();
			emit chatWidgetActivated(chat);
		}
		kdebugf2();
	}
}

void TabWidget::mouseDoubleClickEvent(QMouseEvent *e)
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
	delete currentWidget();
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

	OpenChatButton->setIcon(KaduIcon("internet-group-chat").icon());
	CloseChatButton->setIcon(KaduIcon("kadu_icons/tab-remove").icon());

	setTabsClosable(config_file.readBoolEntry("Tabs", "CloseButtonOnTab"));
	config_oldStyleClosing = config_file.readBoolEntry("Tabs", "OldStyleClosing");

	bool isOpenChatButtonEnabled = (cornerWidget(Qt::TopLeftCorner) == OpenChatButton);
	bool shouldEnableOpenChatButton = config_file.readBoolEntry("Tabs", "OpenChatButton");
	bool isCloseButtonEnabled = (cornerWidget(Qt::TopRightCorner) == CloseChatButton);
	bool shouldEnableCloseButton = config_file.readBoolEntry("Tabs", "CloseButton");

	if (isOpenChatButtonEnabled != shouldEnableOpenChatButton)
	{
		OpenChatButton->setVisible(shouldEnableOpenChatButton);
		setCornerWidget(shouldEnableOpenChatButton ? OpenChatButton : 0, Qt::TopLeftCorner);
	}

	if (isCloseButtonEnabled != shouldEnableCloseButton)
	{
		CloseChatButton->setVisible(shouldEnableCloseButton);
		setCornerWidget(shouldEnableCloseButton ? CloseChatButton : 0, Qt::TopRightCorner);
	}
}

TabBar::TabBar(QWidget *parent) :
		QTabBar(parent)
{
}

void TabBar::mousePressEvent(QMouseEvent *e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::RightButton)
		emit contextMenu(tabAt(e->pos()), mapToGlobal(e->pos()));

	QTabBar::mousePressEvent(e);
}

void TabBar::mouseReleaseEvent(QMouseEvent *e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::MidButton)
		emit tabCloseRequested(tabAt(e->pos()));
	QTabBar::mouseReleaseEvent(e);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent *e)
{
	kdebugf();
	// w celu ulatwienia sobie zadania przekazujemy zdarzenie dalej- tu klasie tabdialog
	emit mouseDoubleClickEventSignal(e);
	kdebugf2();
}
