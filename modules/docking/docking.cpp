/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include "gui/widgets/chat-widget-manager.h"

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#endif

#ifdef Q_WS_X11
#include "x11tools.h"
// TODO: hack :/
#undef Status
#endif
#include "activate.h"
#include "accounts/account-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "debug.h"

#ifdef Q_OS_MAC
#include "mac_docking_helper.h"
extern void qt_mac_set_dock_menu(QMenu *);
#endif

#include "docker.h"

#include "docking.h"

DockingManager * DockingManager::Instance = 0;

void DockingManager::createInstance()
{
	if (!Instance)
		Instance = new DockingManager();
}

void DockingManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

DockingManager * DockingManager::instance()
{
	return Instance;
}

DockingManager::DockingManager() :
		CurrentDocker(0), newMessageIcon(StaticEnvelope), icon_timer(new QTimer(this)), blink(false)
{
	kdebugf();

	createDefaultConfiguration();

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
		this, SLOT(statusPixmapChanged(const QIcon &)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(pendingMessageAdded()));
	connect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(pendingMessageDeleted()));

	connect(Core::instance(), SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(searchingForTrayPosition(QPoint&)));

	DockMenu = new QMenu();

#ifdef Q_OS_MAC
	OpenChatAction = new QAction(IconsManager::instance()->iconByPath("internet-group-chat"),
		tr("Show Pending Messages"), this);
	connect(OpenChatAction, SIGNAL(triggered()), ChatWidgetManager::instance(),
		SLOT(openPendingMsgs()));

	MacDockMenu = new QMenu();
	qt_mac_set_dock_menu(MacDockMenu);
#endif
	CloseKaduAction = new QAction(IconsManager::instance()->iconByPath("application-exit"), tr("&Exit Kadu"), this);
	connect(CloseKaduAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	configurationUpdated();

	updateContextMenu();

	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();

	disconnect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
		this, SLOT(statusPixmapChanged(const QIcon &)));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(pendingMessageAdded()));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(pendingMessageDeleted()));

//	disconnect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	disconnect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	delete DockMenu;
	DockMenu = 0;
#ifdef Q_OS_MAC
	delete MacDockMenu;
	MacDockMenu = 0;
#endif

	delete icon_timer;
	icon_timer = 0;
}

void DockingManager::changeIcon()
{
	kdebugf();
	if (!PendingMessagesManager::instance()->hasPendingMessages() && !icon_timer->isActive())
		return;

	switch (newMessageIcon)
	{
		case AnimatedEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayMovie(IconsManager::instance()->iconPath("protocols/common/16x16/message_anim.gif"));
			break;
		case StaticEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayIcon(IconsManager::instance()->iconByPath("protocols/common/message"));
			break;
		case BlinkingEnvelope:
			if (!blink)
			{
				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(IconsManager::instance()->iconByPath("protocols/common/message"));
				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = true;
			}
			else
			{
				Account account = AccountManager::instance()->defaultAccount();
				if (account.isNull() || !account.protocolHandler())
					return;

				const Status &stat = account.protocolHandler()->status();

				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(account.protocolHandler()->statusIcon(stat));

				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = false;
			}
			break;
	}
}

void DockingManager::pendingMessageAdded()
{
	changeIcon();
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->overlay(QString::number(PendingMessagesManager::instance()->pendingMessages().count()));
	MacDockingHelper::instance()->startBounce();
#endif
}

void DockingManager::pendingMessageDeleted()
{
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->removeOverlay();
	MacDockingHelper::instance()->stopBounce();
#endif
	if (!PendingMessagesManager::instance()->hasPendingMessages())
	{
		Account account = AccountManager::instance()->defaultAccount();
		if (account.isNull() || !account.protocolHandler())
			return;

		const Status &stat = account.protocolHandler()->status();
		if (CurrentDocker)
			CurrentDocker->changeTrayIcon(account.protocolHandler()->statusIcon(stat));
	}
}

void DockingManager::defaultToolTip()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		Status status = AccountManager::instance()->status();

		QString tiptext;
		tiptext.append(tr("Current status:\n%1")
			.arg(qApp->translate("@default", Status::name(status).toLocal8Bit().data())));

		if (!status.description().isEmpty())
			tiptext.append(tr("\n\nDescription:\n%2").arg(status.description()));

		if (CurrentDocker)
			CurrentDocker->changeTrayTooltip(tiptext);
	}
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == Qt::MidButton)
	{
		emit mousePressMidButton();
		ChatWidgetManager::instance()->openPendingMsgs(true);
		return;
	}

	if (e->button() == Qt::LeftButton)
	{
		QWidget *kadu = Core::instance()->kaduWindow();

		emit mousePressLeftButton();
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());

		if (PendingMessagesManager::instance()->hasPendingMessages() && (e->modifiers() != Qt::ControlModifier))
		{
			ChatWidgetManager::instance()->openPendingMsgs(true);
			return;
		}

		if (kadu->isMinimized())
		{
			kadu->showNormal();
			_activateWindow(kadu);
			return;
		}
		else if (kadu->isVisible() && _isActiveWindow(kadu))
			kadu->hide();
		else
		{
			kadu->show();
			_activateWindow(kadu);
		}
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

void DockingManager::statusPixmapChanged(const QIcon &icon)
{
	kdebugf();

	if (CurrentDocker)
		CurrentDocker->changeTrayIcon(icon);

	defaultToolTip();
	changeIcon();
#ifdef Q_OS_MAC
	qApp->setWindowIcon(icon);
#endif
}

void DockingManager::searchingForTrayPosition(QPoint &point)
{
	if (CurrentDocker)
		point = CurrentDocker->trayPosition();

}

QIcon DockingManager::defaultPixmap()
{
	Account account = AccountManager::instance()->defaultAccount();
	if (account.isNull() || !account.protocolHandler())
		return StatusContainerManager::instance()->statusIcon();

	return account.protocolHandler()->statusIcon(account.protocolHandler()->status());
}

void DockingManager::setDocker(Docker *docker)
{
	CurrentDocker = docker;

	if (CurrentDocker)
	{
		changeIcon();
		defaultToolTip();
		if (config_file.readBoolEntry("General", "RunDocked"))
			Core::instance()->setShowMainWindowOnStart(false);
		Core::instance()->kaduWindow()->setDocked(true);
	}
	else
	{
 		if (!Core::instance()->isClosing())
			Core::instance()->kaduWindow()->show();
		Core::instance()->kaduWindow()->setDocked(false);
	}
}

void DockingManager::updateContextMenu()
{
	DockMenu->clear();
#ifdef Q_OS_MAC
	MacDockMenu->clear();
	DockMenu->addAction(OpenChatAction);
#endif

	qDeleteAll(StatusContainerMenus.values());
	StatusContainerMenus.clear();

	int statusContainersCount = StatusContainerManager::instance()->statusContainers().count();

	if (statusContainersCount == 1)
	{
		new StatusMenu(StatusContainerManager::instance()->statusContainers()[0], DockMenu);
#ifdef Q_OS_MAC
		new StatusMenu(StatusContainerManager::instance()->statusContainers()[0], MacDockMenu);
#endif
	}
	else
	{
		foreach (StatusContainer *container, StatusContainerManager::instance()->statusContainers())
		{
			QMenu *menu = new QMenu(container->statusContainerName());
			menu->setIcon(container->statusIcon());
			new StatusMenu(container, menu);
			StatusContainerMenus[container] = DockMenu->addMenu(menu);
			connect(container, SIGNAL(statusChanged()), this, SLOT(containerStatusChanged()));
		}

		if (statusContainersCount > 1)
			containersSeparator = DockMenu->addSeparator();

		new StatusMenu(StatusContainerManager::instance(), DockMenu);
#ifdef Q_OS_MAC
		new StatusMenu(StatusContainerManager::instance(), MacDockMenu);
#endif
	}

	DockMenu->addAction(CloseKaduAction);
}

void DockingManager::containerStatusChanged()
{
	StatusContainer *container;
	if (sender() && (container = dynamic_cast<StatusContainer *>(sender())) && StatusContainerMenus[container])
		StatusContainerMenus[container]->setIcon(container->statusIcon());
}

void DockingManager::statusContainerRegistered(StatusContainer *statusContainer)
{
	if (StatusContainerManager::instance()->statusContainers().count() < 3)
		updateContextMenu();
	else
	{
		QMenu *menu = new QMenu(statusContainer->statusContainerName());
		menu->setIcon(statusContainer->statusIcon());
		new StatusMenu(statusContainer, menu);
		StatusContainerMenus[statusContainer] = DockMenu->insertMenu(containersSeparator, menu);
		connect(statusContainer, SIGNAL(statusChanged()), this, SLOT(containerStatusChanged()));
	}
}

void DockingManager::statusContainerUnregistered(StatusContainer *statusContainer)
{
	if (StatusContainerManager::instance()->statusContainers().count() < 2)
		updateContextMenu();
	else
	{
		QAction *menuAction = StatusContainerMenus[statusContainer];
		if (!menuAction)
			    return;

		menuAction->menu()->clear();
		StatusContainerMenus.remove(statusContainer);
		DockMenu->removeAction(menuAction);
		delete menuAction;
	}
}

void DockingManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
	{
		if (CurrentDocker)
			CurrentDocker->changeTrayTooltip(QString::null);
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
