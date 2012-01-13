/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QStringList>
#include <QtGui/QCloseEvent>
#include <QtGui/QKeyEvent>

#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/kadu-window.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
#include "misc/misc.h"
#include "activate.h"
#include "debug.h"
#include "kadu-application.h"

#include "single-window.h"

SingleWindowManager::SingleWindowManager(QObject *parent) :
		ConfigurationUiHandler(parent)
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

	connect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	/* conquer all already open chats ;) */
	foreach (ChatWidget *chatWidget, ChatWidgetManager::instance()->chats())
	{
		if (chatWidget->parentWidget())
			chatWidget->parentWidget()->deleteLater();
		else
			chatWidget->kaduRestoreGeometry();

		bool dummy;
		onNewChat(chatWidget, dummy);
	}

	setFocusProxy(kadu);
	kadu->show();
	kadu->setFocus();
	show();
}

SingleWindow::~SingleWindow()
{
	KaduWindow *kadu = Core::instance()->kaduWindow();

	saveWindowGeometry(this, "SingleWindow", "WindowGeometry");
	config_file.writeEntry("SingleWindow", "KaduWindowWidth", kadu->width());

	disconnect(ChatWidgetManager::instance(), SIGNAL(handleNewChatWidget(ChatWidget *,bool &)),
			this, SLOT(onNewChat(ChatWidget *,bool &)));

	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	disconnect(tabs, SIGNAL(currentChanged(int)), this, SLOT(onTabChange(int)));

	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(onkaduKeyPressed(QKeyEvent *)));

	if (!Core::instance()->isClosing())
	{
		for (int i = tabs->count()-1; i >= 0; --i)
		{
			ChatWidget *chatWidget = static_cast<ChatWidget *>(tabs->widget(i));
			const Chat &chat = chatWidget->chat();
			tabs->removeTab(i);
			delete chatWidget;
			ChatWidgetManager::instance()->byChat(chat, true);
		}
	}

	kadu->setParent(0);
	if (!Core::instance()->isClosing())
	{
		loadWindowGeometry(kadu, "General", "Geometry", 0, 50, 205, 465);
		kadu->show();
	}
}

void SingleWindow::changeEvent(QEvent *event)
{
	QMainWindow::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		ChatWidget *chatWidget = static_cast<ChatWidget *>(tabs->currentWidget());
		if (chatWidget && _isActiveWindow(this))
		{
			MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
			updateTabIcon(chatWidget);
			updateTabName(chatWidget);
		}
	}
}

void SingleWindow::changeRosterPos(int newRosterPos)
{
	rosterPos = newRosterPos;
	split->insertWidget(rosterPos, Core::instance()->kaduWindow());
}

void SingleWindow::onNewChat(ChatWidget *chatWidget, bool &handled)
{
	handled = true;
	chatWidget->setContainer(this);

	tabs->addTab(chatWidget, chatWidget->icon(), QString());
	updateTabName(chatWidget);

	connect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
		this, SLOT(onChatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	connect(chatWidget, SIGNAL(closed()), this, SLOT(closeChat()));
	connect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	connect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));
}

void SingleWindow::updateTabIcon(ChatWidget *chatWidget)
{
	Q_ASSERT(chatWidget);

	const int i = tabs->indexOf(chatWidget);
	if (-1 == i)
		return;

	if (chatWidget->chat().unreadMessagesCount() > 0)
		tabs->setTabIcon(i, KaduIcon("protocols/common/message").icon());
	else
		tabs->setTabIcon(i, chatWidget->icon());
}

// TODO: share with tabs
void SingleWindow::updateTabName(ChatWidget *chatWidget)
{
	const int i = tabs->indexOf(chatWidget);
	if (-1 == i)
		return;

	const Chat &chat = chatWidget->chat();
	QString baseTabName;
	if (!chat.display().isEmpty())
		baseTabName = chat.display();
	else
	{
		int contactsCount = chat.contacts().count();
		if (contactsCount > 1)
			baseTabName = tr("Conference [%1]").arg(contactsCount);
		else
			baseTabName = chat.name();
	}

	if (config_file.readBoolEntry("SingleWindow", "NumMessagesInTab", false) && chat.unreadMessagesCount() > 0)
	{
		tabs->setTabText(i, QString("%1 [%2]").arg(baseTabName).arg(chat.unreadMessagesCount()));
		tabs->setTabToolTip(i, QString("%1\n%2 new message(s)").arg(chatWidget->title()).arg(chat.unreadMessagesCount()));
	}
	else
	{
		tabs->setTabText(i, baseTabName);
		tabs->setTabToolTip(i, chatWidget->title());
	}
}

void SingleWindow::closeTab(int index)
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(tabs->widget(index));

	disconnect(chatWidget->edit(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
		this, SLOT(onChatKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	disconnect(chatWidget, SIGNAL(closed()), this, SLOT(closeChat()));
	disconnect(chatWidget, SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
	disconnect(chatWidget, SIGNAL(titleChanged(ChatWidget * , const QString &)),
			this, SLOT(onTitleChanged(ChatWidget *, const QString &)));

	tabs->widget(index)->deleteLater();
	tabs->removeTab(index);
}

void SingleWindow::closeEvent(QCloseEvent *event)
{
	// do not block window closing when session is about to close
	if (Core::instance()->application()->sessionClosing())
	{
		QMainWindow::closeEvent(event);
		return;
	}

	if (Core::instance()->kaduWindow()->docked())
	{
		event->ignore();
		hide();
	}
	else
	{
		QMainWindow::closeEvent(event);
		Core::instance()->application()->quit();
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

void SingleWindow::activateChatWidget(ChatWidget *chatWidget)
{
	int index = tabs->indexOf(chatWidget);
	if (index < 0)
		return;

	_activateWindow(this);

	tabs->setCurrentIndex(index);
	chatWidget->edit()->setFocus();
}

void SingleWindow::alertChatWidget(ChatWidget *chatWidget)
{
	Q_ASSERT(chatWidget);

	if (isChatWidgetActive(chatWidget))
	{
		MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
		return;
	}

	updateTabIcon(chatWidget);
	updateTabName(chatWidget);
}

void SingleWindow::closeChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget)
		return;

	int index = tabs->indexOf(chatWidget);
	if (index >= 0)
		closeTab(index);
}

bool SingleWindow::isChatWidgetActive(ChatWidget *chatWidget)
{
	return tabs->currentWidget() == chatWidget && _isWindowActiveOrFullyVisible(this);
}

void SingleWindow::onTabChange(int index)
{
	if (index == -1)
		return;

	ChatWidget *chatWidget = (ChatWidget *)tabs->widget(index);
	MessageManager::instance()->markAllMessagesAsRead(chatWidget->chat());
	updateTabIcon(chatWidget);
	updateTabName(chatWidget);
}

void SingleWindow::onkaduKeyPressed(QKeyEvent *e)
{
	/* unfortunatelly does not work correctly */
	if (HotKey::shortCut(e, "ShortCuts", "FocusOnRosterTab"))
	{
		ChatWidget *chatWidget = static_cast<ChatWidget *>(tabs->currentWidget());
		if (chatWidget)
			chatWidget->edit()->setFocus();
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

void SingleWindow::onIconChanged()
{
	ChatWidget *chatWidget = static_cast<ChatWidget *>(sender());
	updateTabIcon(chatWidget);
}

void SingleWindow::onTitleChanged(ChatWidget *chatWidget, const QString &newTitle)
{
	Q_UNUSED(newTitle)

	int chatIndex = tabs->indexOf(chatWidget);

	if (-1 == chatIndex || !chatWidget)
		return;

	updateTabName(chatWidget);
}

void SingleWindow::closeChat()
{
	QObject *chat = sender();
	if (chat)
		chat->deleteLater();
}
