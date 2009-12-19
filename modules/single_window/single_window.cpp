/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef Q_WS_HILDON
# include <QtGui/QMenu>
# include <QtGui/QMenuBar>
#endif
#include <QtCore/QStringList>

#include "kadu.h"
#include "misc.h"
#include "debug.h"
#include "hot_key.h"
#include "config_file.h"
#include "custom_input.h"
#include "icons_manager.h"
#include "single_window.h"

extern "C" KADU_EXPORT int single_window_init(bool firstLoad)
{
	kdebugf();

	singleWindowManager = new SingleWindowManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/single_window.ui"),
		singleWindowManager);
	kdebugf2();

	return 0;
}
extern "C" KADU_EXPORT void single_window_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/single_window.ui"),
		singleWindowManager);
	delete singleWindowManager;
	singleWindowManager = NULL;

	kdebugf2();
}

SingleWindowManager::SingleWindowManager()
{
	singleWindow = new SingleWindow();
}

SingleWindowManager::~SingleWindowManager()
{
	delete singleWindow;
}

void SingleWindowManager::configurationUpdated()
{
	if (singleWindow->rosterPosition() != config_file.readNumEntry("SingleWindow", "RosterPosition", 0))
	{
		delete singleWindow;
		singleWindow = new SingleWindow();
	}
}

SingleWindow::SingleWindow()
{
	split = new QSplitter(Qt::Horizontal, this);

	tabs = new QTabWidget(this);
	tabs->setTabsClosable(true);

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

#ifdef Q_WS_HILDON
	menuBar()->addMenu(kadu->mainMenu());

	if (kadu->width() >= 250)
		kadu->resize(250, kadu->height());
#endif

	loadWindowGeometry(this, "SingleWindow", "WindowGeometry", 0, 0, 600, 600);

	if (rosterPos == 0)
	{
		splitSizes.append(kadu->width());
		splitSizes.append(width() - kadu->width());
	}
	else
	{
		splitSizes.append(width() - kadu->width());
		splitSizes.append(kadu->width());
	}
	split->setSizes(splitSizes);

	setWindowTitle(kadu->windowTitle());

	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	connect(chat_manager, SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	connect(chat_manager, SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	connect(kadu, SIGNAL(shown()), this, SLOT(show()));
	connect(kadu, SIGNAL(hiding()), this, SLOT(hide()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	show();
}

SingleWindow::~SingleWindow()
{
	int i;
	for (i = 0; i < tabs->count(); ++i)
	{
		tabs->removeTab(i);
		delete tabs->widget(i);
	}

	split->setSizes(splitSizes);

	// reparent kadu
	kadu->setParent(NULL);

	saveWindowGeometry(this, "SingleWindow", "WindowGeometry");

	disconnect(chat_manager, SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	disconnect(chat_manager, SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	disconnect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	disconnect(kadu, SIGNAL(shown()), this, SLOT(show()));
	disconnect(kadu, SIGNAL(hiding()), this, SLOT(hide()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));
}

void SingleWindow::onNewChat(ChatWidget *w, bool &handled)
{
	handled = true;
	onOpenChat(w);
}

void SingleWindow::onOpenChat(ChatWidget *w)
{
	QStringList nicks = w->users()->altNicks();
	QString title = nicks[0];
	if (nicks.count() > 1)
		title.append(", ...");

	tabs->addTab(w, w->icon(), title);

	connect(w, SIGNAL(messageReceived(ChatWidget *)),
		this, SLOT(onNewMessage(ChatWidget *)));

	connect(w, SIGNAL(keyPressed(QKeyEvent*, ChatWidget*, bool&)),
		this, SLOT(onChatKeyPressed(QKeyEvent*, ChatWidget*, bool&)));
}

void SingleWindow::closeTab(int index)
{
	tabs->widget(index)->deleteLater();
	tabs->removeTab(index);
}

void SingleWindow::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
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

void SingleWindow::onNewMessage(ChatWidget *w)
{
	if (w != tabs->currentWidget())
	{
		int index = tabs->indexOf(w);
		tabs->setTabIcon(index, icons_manager->loadIcon("Message"));

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

void SingleWindow::onChatKeyPressed(QKeyEvent* e, ChatWidget* w, bool &handled)
{
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
		if (sizes[0] != 0)
			sizes[0] = 0;
		else
			sizes = splitSizes;
		split->setSizes(sizes);
		handled = true;
	}
	else if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		kadu->userBox()->setFocus();
		handled = true;
	}
}

SingleWindowManager *singleWindowManager = NULL;
