/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "misc.h"

#include "single_window.h"

extern "C" KADU_EXPORT int single_window_init(bool firstLoad)
{
	kdebugf();

	singleWindow = new SingleWindow();
	kdebugf2();

	return 0;
}
extern "C" KADU_EXPORT void single_window_close()
{
	kdebugf();

	delete singleWindow;
	singleWindow = NULL;

	kdebugf2();
}

SingleWindow::SingleWindow()
{
	split = new QSplitter(Qt::Horizontal, this);
	split->addWidget(kadu);

	tabs = new QTabWidget(this);
	tabs->setTabsClosable(true);
	split->addWidget(tabs);

	loadWindowGeometry(this, "SingleWindow", "WindowGeometry", 0, 0, 600, 600);

	QList<int> splitSizes;

#ifdef Q_WS_HILDON
	if (kadu->width() >= 300)
		kadu->resize(300, kadu->height());
#endif
	splitSizes.append(kadu->width());
	splitSizes.append(width() - kadu->width());
	split->setSizes(splitSizes);

	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	connect(chat_manager, SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	connect(chat_manager, SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	connect(kadu, SIGNAL(shown()), this, SLOT(show()));
	connect(kadu, SIGNAL(hiding()), this, SLOT(hide()));

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

	// reparent kadu
	kadu->setParent(NULL);

	saveWindowGeometry(this, "SingleWindow", "WindowGeometry");

	disconnect(chat_manager, SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));
	disconnect(chat_manager, SIGNAL(chatWidgetOpen(ChatWidget *)),
			this, SLOT(onOpenChat(ChatWidget *)));

	disconnect(kadu, SIGNAL(shown()), this, SLOT(show()));
	disconnect(kadu, SIGNAL(hiding()), this, SLOT(hide()));
}

void SingleWindow::onNewChat(ChatWidget *w, bool &handled)
{
	handled = true;
	QString title = w->caption();
	if (title.length() > 20)
		title = title.left(17) + "...";
	tabs->addTab(w, w->icon(), title);
}

void SingleWindow::onOpenChat(ChatWidget *w)
{
	QString title = w->caption();
	if (title.length() > 20)
		title = title.left(17) + "...";
	tabs->addTab(w, w->icon(), title);
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

SingleWindow *singleWindow = NULL;
