/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * Copyright 2011 Adam "Vertex" Makświej (vertexbz@gmail.com)
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

#include "gui/widgets/chat-widget-manager.h"

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "activate.h"
#include "debug.h"

#ifdef Q_OS_MAC
#include "notify/notification-manager.h"
#include "mac_docking_helper.h"
extern void qt_mac_set_dock_menu(QMenu *);
#endif

#include "docker.h"

#include <gui/status-icon.h>
#include "docking.h"

DockingManager * DockingManager::Instance = 0;

void DockingManager::createInstance()
{
	if (!Instance)
		Instance = new DockingManager();
#ifdef Q_OS_MAC
	MacDockingHelper::instance();
#endif
}

void DockingManager::destroyInstance()
{
#ifdef Q_OS_MAC
	MacDockingHelper::destroyInstance();
#endif
	delete Instance;
	Instance = 0;
}

DockingManager * DockingManager::instance()
{
	return Instance;
}

DockingManager::DockingManager() :
		CurrentDocker(0), DockMenuNeedsUpdate(true), AllAccountsMenu(0),
		newMessageIcon(StaticEnvelope), icon_timer(new QTimer(this)), blink(false)
{
	kdebugf();

#ifdef Q_WS_X11
	KaduWindowLastTimeVisible = true;
#endif

	createDefaultConfiguration();

	Icon = new StatusIcon(StatusContainerManager::instance(), this);
	connect(Icon, SIGNAL(iconUpdated(KaduIcon)), this, SLOT(statusIconChanged(KaduIcon)));

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded()));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved()));

	connect(Core::instance(), SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(searchingForTrayPosition(QPoint&)));

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));

	DockMenu = new QMenu();
	DockMenu->setSeparatorsCollapsible(true);
	connect(DockMenu, SIGNAL(aboutToShow()), this, SLOT(contextMenuAboutToBeShown()));

#ifdef Q_OS_MAC
	MacDockMenu = new QMenu();
	qt_mac_set_dock_menu(MacDockMenu);
	MacDockMenu->setSeparatorsCollapsible(true);
#endif

#ifdef Q_WS_X11
	ShowKaduAction = new QAction(tr("&Restore"), this);
	connect(ShowKaduAction, SIGNAL(triggered()), this, SLOT(showKaduWindow()));

	HideKaduAction = new QAction(tr("&Minimize"), this);
	connect(HideKaduAction, SIGNAL(triggered()), this, SLOT(hideKaduWindow()));
#endif

	CloseKaduAction = new QAction(KaduIcon("application-exit").icon(), tr("&Exit Kadu"), this);
	connect(CloseKaduAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	configurationUpdated();

	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();

	icon_timer->stop();

	delete DockMenu;
	DockMenu = 0;
#ifdef Q_OS_MAC
	delete MacDockMenu;
	MacDockMenu = 0;
#endif
}

void DockingManager::changeIcon()
{
	kdebugf();
	if (!MessageManager::instance()->hasUnreadMessages() && !icon_timer->isActive())
		return;

	switch (newMessageIcon)
	{
		case AnimatedEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayMovie(KaduIcon("protocols/common/message_anim", "16x16").fullPath());
			break;
		case StaticEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayIcon(KaduIcon("protocols/common/message"));
			break;
		case BlinkingEnvelope:
			if (!blink)
			{
				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(KaduIcon("protocols/common/message"));
				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = true;
			}
			else
			{
				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(StatusContainerManager::instance()->statusIcon());

				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = false;
			}
			break;
	}
}

void DockingManager::unreadMessageAdded()
{
	changeIcon();
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->overlay(MessageManager::instance()->unreadMessagesCount());
	if (!NotificationManager::instance()->silentMode())
		MacDockingHelper::instance()->startBounce();
#endif
}

void DockingManager::unreadMessageRemoved()
{
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->overlay(MessageManager::instance()->unreadMessagesCount());
	MacDockingHelper::instance()->stopBounce();
#endif
	if (!MessageManager::instance()->hasUnreadMessages())
		if (CurrentDocker)
			CurrentDocker->changeTrayIcon(defaultIcon());
}

void DockingManager::defaultToolTip()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		Status status = StatusContainerManager::instance()->status();

		QString tiptext;
		tiptext.append(tr("Current status:\n%1").arg(status.displayName()));

		if (!status.description().isEmpty())
			tiptext.append(tr("\n\nDescription:\n%2").arg(status.description()));

		if (CurrentDocker)
			CurrentDocker->changeTrayTooltip(tiptext);
	}
}

void DockingManager::showKaduWindow()
{
	_activateWindow(Core::instance()->kaduWindow());
}

void DockingManager::hideKaduWindow()
{
	KaduWindow *kaduWindow = Core::instance()->kaduWindow();
	if (kaduWindow->docked())
		kaduWindow->window()->hide();
}

void DockingManager::openUnreadMessages()
{
	const Message &message = MessageManager::instance()->unreadMessage();
	ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat(), true);
	if (chatWidget)
		chatWidget->activate();
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == Qt::MidButton)
	{
		emit mousePressMidButton();
		openUnreadMessages();
		return;
	}

	if (e->button() == Qt::LeftButton)
	{
		QWidget *kadu = Core::instance()->kaduWindow()->window();

		emit mousePressLeftButton();
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());

		if (MessageManager::instance()->hasUnreadMessages() && (e->modifiers() != Qt::ControlModifier))
		{
			openUnreadMessages();
			return;
		}

		if (kadu->isMinimized() || !kadu->isVisible()
#ifndef Q_WS_WIN
				// NOTE: It won't work as expected on Windows since when you click on tray icon,
				// the tray will become active and any other window will loose focus.
				// See bug #1915.
				|| !_isActiveWindow(kadu)
#endif
				)
			showKaduWindow();
		else
			hideKaduWindow();

		return;
	}

	if (e->button() == Qt::RightButton)
	{
		emit mousePressRightButton();
		//showPopupMenu(dockMenu);
		return;
	}
	kdebugf2();
}

void DockingManager::statusIconChanged(const KaduIcon &icon)
{
	kdebugf();

	if (MessageManager::instance()->hasUnreadMessages() || icon_timer->isActive())
		return;

	if (CurrentDocker)
		CurrentDocker->changeTrayIcon(icon);

	defaultToolTip();
#ifdef Q_OS_MAC
	qApp->setWindowIcon(icon.icon());
#endif
}

void DockingManager::searchingForTrayPosition(QPoint &point)
{
	if (CurrentDocker)
		point = CurrentDocker->trayPosition();
}

KaduIcon DockingManager::defaultIcon()
{
	return StatusContainerManager::instance()->statusIcon();
}

void DockingManager::setDocker(Docker *docker)
{
	CurrentDocker = docker;

	if (CurrentDocker)
	{
		changeIcon();
		defaultToolTip();
#ifndef Q_OS_MAC
		if (config_file.readBoolEntry("General", "RunDocked"))
			Core::instance()->setShowMainWindowOnStart(false);
		Core::instance()->kaduWindow()->setDocked(true);
	}
	else
	{
 		if (!Core::instance()->isClosing())
			Core::instance()->kaduWindow()->window()->show();
		Core::instance()->kaduWindow()->setDocked(false);
#endif
	}
}

void DockingManager::contextMenuAboutToBeShown()
{
	if (DockMenuNeedsUpdate
#ifdef Q_WS_X11
			|| Core::instance()->kaduWindow()->window()->isVisible() != KaduWindowLastTimeVisible
#endif
			)
		doUpdateContextMenu();
}

void DockingManager::updateContextMenu()
{
	DockMenuNeedsUpdate = true;
}

void DockingManager::doUpdateContextMenu()
{
	if (AllAccountsMenu)
	{
		AllAccountsMenu->deleteLater();
		AllAccountsMenu = 0;
	}

	DockMenu->clear();
#ifdef Q_OS_MAC
	MacDockMenu->clear();
#endif

	qDeleteAll(StatusContainerMenus.values());
	StatusContainerMenus.clear();

	int statusContainersCount = StatusContainerManager::instance()->statusContainers().count();

	if (1 == statusContainersCount)
	{
		AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), false, DockMenu);
#ifdef Q_OS_MAC
		AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), false, MacDockMenu);
#endif
		connect(AllAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(updateContextMenu()));
	}
	else
	{
		foreach (StatusContainer *container, StatusContainerManager::instance()->statusContainers())
		{
			QMenu *menu = new QMenu(container->statusContainerName(), DockMenu);
			menu->setIcon(container->statusIcon().icon());
			new StatusMenu(container, false, menu);
			StatusContainerMenus[container] = DockMenu->addMenu(menu);
			connect(container, SIGNAL(statusUpdated()), this, SLOT(containerStatusChanged()));
		}

		if (statusContainersCount > 1)
			containersSeparator = DockMenu->addSeparator();

		if (statusContainersCount > 0)
		{
			AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), true, DockMenu);
#ifdef Q_OS_MAC
			AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), true, MacDockMenu);
#endif
			connect(AllAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(updateContextMenu()));
		}
	}

	if (!ModulesActions.isEmpty())
	{
		DockMenu->addSeparator();

		foreach (QAction *action, ModulesActions)
			DockMenu->addAction(action);
	}

	DockMenu->addSeparator();

#ifdef Q_WS_X11
	KaduWindowLastTimeVisible = Core::instance()->kaduWindow()->window()->isVisible();
	DockMenu->addAction(KaduWindowLastTimeVisible ? HideKaduAction : ShowKaduAction);
#endif
	DockMenu->addAction(CloseKaduAction);

	DockMenuNeedsUpdate = false;
}

void DockingManager::containerStatusChanged()
{
	StatusContainer *container;
	if (sender() && (container = qobject_cast<StatusContainer *>(sender())) && StatusContainerMenus[container])
		StatusContainerMenus[container]->setIcon(container->statusIcon().icon());
}

void DockingManager::iconThemeChanged()
{
	QMapIterator<StatusContainer *, QAction *> i(StatusContainerMenus);
	while (i.hasNext())
	{
		i.next();
		i.value()->setIcon(i.key()->statusIcon().icon());
	}
}

void DockingManager::statusContainerRegistered(StatusContainer *statusContainer)
{
	Q_UNUSED(statusContainer)

	updateContextMenu();
}

void DockingManager::statusContainerUnregistered(StatusContainer *statusContainer)
{
	Q_UNUSED(statusContainer)

	updateContextMenu();
}

void DockingManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
	{
		if (CurrentDocker)
			CurrentDocker->changeTrayTooltip(QString());
	}

	IconType it = (IconType)config_file.readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
}

void DockingManager::createDefaultConfiguration()
{
	config_file.addVariable("General", "RunDocked", false);
	config_file.addVariable("General", "ShowTooltipInTray", true);
	config_file.addVariable("Look", "NewMessageIcon", 0);
}

void DockingManager::registerModuleAction(QAction *action)
{
	if (ModulesActions.contains(action))
		return;

	ModulesActions.append(action);
	updateContextMenu();
}

void DockingManager::unregisterModuleAction(QAction *action)
{
	if (!ModulesActions.contains(action))
		return;

	ModulesActions.removeAll(action);
	updateContextMenu();
}

#ifdef Q_OS_MAC
void DockingManager::showMinimizedChats()
{
	foreach (ChatWidget *chat, ChatWidgetManager::instance()->chats())
		chat->activate();
}

void DockingManager::dockIconClicked()
{
	QWidget *kadu = Core::instance()->kaduWindow()->window();

	if (MessageManager::instance()->hasUnreadMessages())
	{
		openUnreadMessages();
		return;
	}

	if (kadu->isMinimized())
	{
		kadu->setWindowState(kadu->windowState() & ~Qt::WindowMinimized);
		kadu->show();
		showMinimizedChats();
		return;
	}
	else if (kadu->isVisible())
	{
		//raczej nie bedziemy ukrywac okna klikajac ikonke w docku
		//hideKaduWindow();
	}
	else
	{
		kadu->show();
		showMinimizedChats();
	}
	return;
}
#endif
