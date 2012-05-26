/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Jiri Zamazal (zamazal.jiri@gmail.com)
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtCore/QPoint>
#include <QtCore/QVariant>

#include "chat/chat.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/recent-chat-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/open-chat-with/open-chat-with.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
#include "misc/misc.h"
#include "activate.h"
#include "kadu-application.h"

#include "tabs.h"

#include "tabwidget.h"

TabWidget::TabWidget(TabsManager *manager) : Manager(manager)
{
	setWindowRole("kadu-tabs");

	TabBar *tabbar = new TabBar(this);
	setTabBar(tabbar);

	setAcceptDrops(true);
	setMovable(true);

	setDocumentMode(true);
	setElideMode(Qt::ElideRight);


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

	//widget (container) for buttons with opening conversations
	//both buttons are displayed when checking Show "New Tab" button in configurations
	OpenChatButtonsWidget = new QWidget(this);
	QHBoxLayout *horizontalLayout = new QHBoxLayout;

	horizontalLayout->setSpacing(2);
	horizontalLayout->setContentsMargins(3, 0, 2, 3);

	//button for new chat from last conversations
	OpenRecentChatButton = new QToolButton(OpenChatButtonsWidget);
	OpenRecentChatButton->setIcon(KaduIcon("internet-group-chat").icon());
	OpenRecentChatButton->setAutoRaise(true);
	connect(OpenRecentChatButton, SIGNAL(clicked()), SLOT(newChatFromLastConversation()));

	//button for opening chat
	QToolButton *openChatButton = new QToolButton(OpenChatButtonsWidget);
	openChatButton->setIcon(KaduIcon("mail-message-new").icon());
	openChatButton->setAutoRaise(true);
	connect(openChatButton, SIGNAL(clicked()), SLOT(newChat()));

	horizontalLayout->addWidget(OpenRecentChatButton);
	horizontalLayout->addWidget(openChatButton);

	OpenChatButtonsWidget->setLayout(horizontalLayout);
	OpenChatButtonsWidget->setVisible(false);

	//menu for recent chats
	RecentChatsMenu = new QMenu(this);

	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(checkRecentChats()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(checkRecentChats()));
	connect(RecentChatManager::instance(), SIGNAL(recentChatRemoved(Chat)), this, SLOT(checkRecentChats()));

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

void TabWidget::activateChatWidget(ChatWidget *chatWidget)
{
	int index = indexOf(chatWidget);
	if (index < 0)
		return;

	_activateWindow(this);

	setCurrentIndex(index);
	chatWidget->edit()->setFocus();
}

void TabWidget::alertChatWidget(ChatWidget *chatWidget)
{
	Q_ASSERT(chatWidget);

	if (isChatWidgetActive(chatWidget))
	{
		MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
		return;
	}

	Manager->addChatWidgetToChatWidgetsWithMessage(chatWidget);
}

void TabWidget::closeTab(QWidget *tabWidget)
{
	ChatWidget *chatWidget = qobject_cast<ChatWidget *>(tabWidget);
	if (!chatWidget)
		return;

	if (config_file.readBoolEntry("Chat", "ChatCloseTimer"))
	{
		unsigned int period = config_file.readUnsignedNumEntry("Chat",
			"ChatCloseTimerPeriod", 2);

		if (QDateTime::currentDateTime() < chatWidget->lastReceivedMessageTime().addSecs(period))
		{
			if (!MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), tr("New message received, close window anyway?")))
				return;
		}
	}

	delete chatWidget;
}

void TabWidget::closeChatWidget(ChatWidget *chatWidget)
{
	delete chatWidget;
}

bool TabWidget::isChatWidgetActive(ChatWidget *chatWidget)
{
	return currentWidget() == chatWidget && _isWindowActiveOrFullyVisible(this);
}

void TabWidget::closeEvent(QCloseEvent *e)
{
	// do not block window closing when session is about to close
	if (Core::instance()->application()->sessionClosing())
	{
		QTabWidget::closeEvent(e);
		return;
	}

	//w zaleznosci od opcji w konfiguracji zamykamy wszystkie karty, lub tylko aktywna
	if (config_oldStyleClosing)
		closeTab(currentWidget());
	else
		for (int i = count() - 1; i >= 0; i--)
			closeTab(widget(i));

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
	#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
		#define TAB_SWITCH_MODIFIER "Ctrl"
	#else
		#define TAB_SWITCH_MODIFIER "Alt"
	#endif
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+0")
		setCurrentIndex(count() - 1);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+1")
		setCurrentIndex(0);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+2")
		setCurrentIndex(1);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+3")
		setCurrentIndex(2);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+4")
		setCurrentIndex(3);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+5")
		setCurrentIndex(4);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+6")
		setCurrentIndex(5);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+7")
		setCurrentIndex(6);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+8")
		setCurrentIndex(7);
	else if (HotKey::keyEventToString(e) == TAB_SWITCH_MODIFIER "+9")
		setCurrentIndex(8);
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
	closeTab(widget(id));
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
	if (qobject_cast<FilteredTreeView *>(e->source()) && false)/*UlesDrag::decode(e, ules))*/
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
		ChatWidget *chatWidget = static_cast<ChatWidget *>(currentWidget());
		if (chatWidget && _isActiveWindow(this))
			MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
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

void TabWidget::newChatFromLastConversation()
{
	//load recent chats to popup menu
	RecentChatsMenu->clear();
	foreach (const Chat &chat, RecentChatManager::instance()->recentChats())
		if (!ChatWidgetManager::instance()->byChat(chat, false))
		{
			QAction *action = new QAction(ChatDataExtractor::data(chat, Qt::DisplayRole).toString(), RecentChatsMenu);
			action->setData(QVariant::fromValue<Chat>(chat));
			RecentChatsMenu->addAction(action);
		}
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChat(QAction *)));

	//show last conversations menu under widget with buttons for opening chats
	RecentChatsMenu->popup(OpenChatButtonsWidget->mapToGlobal(QPoint(0, OpenChatButtonsWidget->height())));
}

void TabWidget::openRecentChat(QAction *action)
{
	ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(action->data().value<Chat>(), true);
	if (chatWidget)
		chatWidget->activate();
}

void TabWidget::checkRecentChats()
{
	//check if all recent chats are opened -> disable button
	foreach (const Chat &chat, RecentChatManager::instance()->recentChats())
		if (!ChatWidgetManager::instance()->byChat(chat, false))
		{
			OpenRecentChatButton->setEnabled(true);
			return;
		}
	OpenRecentChatButton->setEnabled(false);
}

void TabWidget::deleteTab()
{
	closeTab(currentWidget());
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

	CloseChatButton->setIcon(KaduIcon("kadu_icons/tab-remove").icon());

	setTabsClosable(config_file.readBoolEntry("Tabs", "CloseButtonOnTab"));
	config_oldStyleClosing = config_file.readBoolEntry("Tabs", "OldStyleClosing");

	bool isOpenChatButtonEnabled = (cornerWidget(Qt::TopLeftCorner) == OpenChatButtonsWidget);
	bool shouldEnableOpenChatButton = config_file.readBoolEntry("Tabs", "OpenChatButton");
	bool isCloseButtonEnabled = (cornerWidget(Qt::TopRightCorner) == CloseChatButton);
	bool shouldEnableCloseButton = config_file.readBoolEntry("Tabs", "CloseButton");

	if (isOpenChatButtonEnabled != shouldEnableOpenChatButton)
	{
		OpenChatButtonsWidget->setVisible(true);
		setCornerWidget(shouldEnableOpenChatButton ? OpenChatButtonsWidget : 0, Qt::TopLeftCorner);
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
