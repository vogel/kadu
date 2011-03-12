/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCloseEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QStringList>

#include "configuration/configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/kadu-window.h"
#include "icons-manager.h"
#include "gui/hot-key.h"
#include "core/core.h"
#include "misc/misc.h"
#include "debug.h"

#include "single_window.h"

extern "C" KADU_EXPORT int single_window_init(bool firstLoad)
{
	kdebugf();

	Q_UNUSED(firstLoad)
	singleWindowManager = new SingleWindowManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/single_window.ui"));
	kdebugf2();

	return 0;
}
extern "C" KADU_EXPORT void single_window_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/single_window.ui"));
	delete singleWindowManager;
	singleWindowManager = NULL;

	kdebugf2();
}

SingleWindowManager::SingleWindowManager()
{
	config_file.addVariable("SingleWindow", "RosterPosition", 0);
	config_file.addVariable("SingleWindow", "KaduWindowWidth", 205);
	singleWindow = new SingleWindow();
}

SingleWindowManager::~SingleWindowManager()
{
	delete singleWindow;
}

void SingleWindowManager::configurationUpdated()
{
	int newRosterPos = config_file.readNumEntry("SingleWindow", "RosterPosition", 0);
	if (singleWindow->rosterPosition() != newRosterPos)
	{
		singleWindow->changeRosterPos(newRosterPos);
	}
}

SingleWindow::SingleWindow()
{
	setWindowRole("kadu-single-window");

	KaduWindow *kadu = Core::instance()->kaduWindow();
	split = new QSplitter(Qt::Horizontal, this);

	tabs = new QTabWidget(this);
	tabs->setTabsClosable(true);

#ifdef Q_WS_MAEMO_5
	tabs->setStyleSheet("QTabBar::tab { height: 56px; }");
#endif

	rosterPos = config_file.readNumEntry("SingleWindow", "RosterPosition", 0);
	if (rosterPos == 0)
	{
		split->addWidget(kadu);
		split->addWidget(tabs);
	}
	else
	{
		split->addWidget(tabs);
		split->addWidget(kadu);
	}

	kadu->setMaximumWidth(QWIDGETSIZE_MAX);
	tabs->setMaximumWidth(QWIDGETSIZE_MAX);
	kadu->setMinimumWidth(170);
	tabs->setMinimumWidth(200);

	loadWindowGeometry(this, "SingleWindow", "WindowGeometry", 0, 0, 800, 440);

	int kaduwidth = config_file.readNumEntry("SingleWindow", "KaduWindowWidth", 205);

#ifdef Q_WS_MAEMO_5
	kaduwidth = kadu->width();
	if (kaduwidth > 250)
		kaduwidth = 250;
	kadu->setFixedWidth(kaduwidth);
#endif

	if (rosterPos == 0)
	{
		splitSizes.append(kaduwidth);
		splitSizes.append(width() - kaduwidth);
	}
	else
	{
		splitSizes.append(width() - kaduwidth);
		splitSizes.append(kaduwidth);
	}
	split->setSizes(splitSizes);

	setWindowTitle(kadu->windowTitle());

	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	connect(ChatWidgetManager::instance(), SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetOpen(ChatWidget *, bool)),
			this, SLOT(onOpenChat(ChatWidget *)));
	connect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
		this, SLOT(onStatusPixmapChanged(const QIcon &)));

	connect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	/* conquer all already open chats ;) */
	foreach (const Chat &c, ChatManager::instance()->allItems())
	{
		ChatWidget *chat = ChatWidgetManager::instance()->byChat(c, true);
		if (chat)
		{
			if (chat->parent())
				chat->parent()->deleteLater();
			else
				chat->kaduRestoreGeometry();
			onOpenChat(chat);
		}
	}

	kadu->show();
	show();
}

SingleWindow::~SingleWindow()
{
	KaduWindow *kadu = Core::instance()->kaduWindow();

	saveWindowGeometry(this, "SingleWindow", "WindowGeometry");
	config_file.writeEntry("SingleWindow", "KaduWindowWidth", kadu->width());

	disconnect(ChatWidgetManager::instance(), SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetOpen(ChatWidget *, bool)),
			this, SLOT(onOpenChat(ChatWidget *)));
	disconnect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
			this, SLOT(onStatusPixmapChanged(const QIcon &)));

	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	disconnect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	if (!Core::instance()->isClosing())
	{
		for (int i = tabs->count()-1; i >= 0; --i)
		{
			ChatWidget *chat = static_cast<ChatWidget *>(tabs->widget(i));
			Chat oldchat = chat->chat();
			tabs->removeTab(i);
			delete chat;
			ChatWidgetManager::instance()->openPendingMessages(oldchat, true);
		}
	}

	kadu->setParent(0);
	if (!Core::instance()->isClosing())
	{
		loadWindowGeometry(kadu, "General", "Geometry", 0, 50, 205, 465);
		kadu->showNormal();
	}
}

void SingleWindow::changeRosterPos(int newRosterPos)
{
	rosterPos = newRosterPos;
	split->insertWidget(rosterPos, Core::instance()->kaduWindow());
}

void SingleWindow::onNewChat(ChatWidget *w, bool &handled)
{
	handled = true;
	QString title = w->chat().name();

	tabs->addTab(w, w->icon(), title);

	connect(w, SIGNAL(messageReceived(Chat)), this, SLOT(onNewMessage(Chat)));
	connect(w->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
		this, SLOT(onChatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	connect(w, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));

	onOpenChat(w);
}

void SingleWindow::onOpenChat(ChatWidget *w)
{
	setWindowState(windowState() & ~Qt::WindowMinimized);
	tabs->setCurrentWidget(w);
	w->edit()->setFocus();
}

void SingleWindow::closeTab(int index)
{
	ChatWidget *w = static_cast<ChatWidget *>(tabs->widget(index));

	disconnect(w, SIGNAL(messageReceived(Chat)), this, SLOT(onNewMessage(Chat)));
	disconnect(w->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
		this, SLOT(onChatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	disconnect(w, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));

	tabs->widget(index)->deleteLater();
	tabs->removeTab(index);
}

void SingleWindow::closeEvent(QCloseEvent *event)
{
	if (Core::instance()->kaduWindow()->docked())
	{
		event->ignore();
		hide();
	}
	else
	{
		QMainWindow::closeEvent(event);
		qApp->quit();
	}
}

void SingleWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape && Core::instance()->kaduWindow()->docked())
	{
		hide();
		return;
	}

	QMainWindow::keyPressEvent(event);
}

void SingleWindow::resizeEvent(QResizeEvent *event)
{
	QSize newSize = event->size();
	split->resize(newSize);
}

void SingleWindow::closeChatWidget(ChatWidget *w)
{
	if (w)
	{
		int index = tabs->indexOf(w);
		if (index >= 0)
			closeTab(index);
	}
}

void SingleWindow::onNewMessage(Chat chat)
{
	ChatWidget *w = ChatWidgetManager::instance()->byChat(chat);
	if (w != tabs->currentWidget())
	{
		int index = tabs->indexOf(w);
		tabs->setTabIcon(index, IconsManager::instance()->iconByPath("protocols/common/message"));

		if (config_file.readBoolEntry("SingleWindow", "NumMessagesInTab", false))
		{
			QString title = tabs->tabText(index);
			int pos = title.indexOf(" [");
			if (pos > -1)
				title.truncate(pos);
			title += QString(" [%1]").arg(w->newMessagesCount());
			tabs->setTabText(index, title);
		}
	}
	else
	{
		w->markAllMessagesRead();
	}
}

void SingleWindow::onTabChange(int index)
{
	if (index == -1)
		return;

	ChatWidget *w = (ChatWidget *)tabs->widget(index);
	tabs->setTabIcon(index, w->icon());

	QString title = tabs->tabText(index);
	int pos = title.indexOf(" [");
	if (pos > -1)
		title.truncate(pos);
	tabs->setTabText(index, title);

	w->markAllMessagesRead();
}

void SingleWindow::onkaduKeyPressed(QKeyEvent *e)
{
	/* unfortunatelly does not work correctly */
	if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		ChatWidget *w = (ChatWidget *)tabs->currentWidget();
		if (w)
		{
			w->edit()->setFocus();
		}
	}
}

void SingleWindow::onChatKeyPressed(QKeyEvent *e, CustomInput *w, bool &handled)
{
	Q_UNUSED(w)

	/* workaround: we're receiving the same key event twice so ignore the duplicate */
	static int duplicate = 0;
	if (duplicate++)
	{
		duplicate = 0;
		handled = false;
		return;
	}

	handled = false;

	if (HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
	{
		int index = tabs->currentIndex();
		if (index > 0)
		{
			tabs->setCurrentIndex(index-1);
		}
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
	{
		int index = tabs->currentIndex();
		if (index < tabs->count())
		{
			tabs->setCurrentIndex(index+1);
		}
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "HideShowRoster"))
	{
		QList<int> sizes = split->sizes();
		if (sizes[rosterPos] != 0)
			sizes[rosterPos] = 0;
		else
			sizes = splitSizes;
		split->setSizes(sizes);
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		//kadu->userBox()->setFocus();//TODO: fixme
		handled = true;
	}
}

void SingleWindow::onStatusPixmapChanged(const QIcon &icon)
{
	setWindowIcon(icon);
}

void SingleWindow::onIconChanged()
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(sender());

	int index;
	if (chatWidget && (index = tabs->indexOf(chatWidget)) != -1)
		tabs->setTabIcon(index, chatWidget->icon());
}


SingleWindowManager *singleWindowManager = NULL;
