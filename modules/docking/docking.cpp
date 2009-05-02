/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

#include "activate.h"
#include "accounts/account-manager.h"
#include "config_file.h"
#include "core/core.h"
#include "debug.h"
#include "gui/windows/kadu-window.h"
#include "gui/widgets/status-menu.h"
#include "icons_manager.h"
#include "main_configuration_window.h"
#include "misc/misc.h"
#include "pending_msgs.h"
#include "protocols/protocol.h"
#include "status_changer.h"

#include "docking.h"

/**
 * @ingroup docking
 * @{
 */
extern "C" KADU_EXPORT int docking_init(bool firstLoad)
{
	docking_manager = new DockingManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/docking.ui"));

	return 0;
}

extern "C" KADU_EXPORT void docking_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/docking.ui"));
	delete docking_manager;
	docking_manager = 0;
}

DockingManager::DockingManager()
	: newMessageIcon(StaticEnvelope), icon_timer(new QTimer()), blink(false)
{
	kdebugf();

	createDefaultConfiguration();

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
		this, SLOT(statusPixmapChanged(const QIcon &)));
	connect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(pendingMessageDeleted()));

//	connect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	DockMenu = new QMenu();
	StatusMenu *statusMenu = new StatusMenu(this);
	statusMenu->addToMenu(DockMenu);
#ifdef Q_OS_MAC
	DockMenu->insertSeparator();
	DockMenu->addAction(icons_manager->loadIcon("OpenChat"), tr("Show Pending Messages"), chat_manager, SLOT(openPendingMsgs()));
#endif
	DockMenu->addSeparator();
	DockMenu->addAction(icons_manager->loadIcon("Exit"), tr("&Exit Kadu"), Core::instance(), SLOT(quit()));

	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	configurationUpdated();

	kdebugf2();
}

void DockingManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
		emit trayTooltipChanged(QString::null);

	IconType it = (IconType)config_file.readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
}

DockingManager::~DockingManager()
{
	kdebugf();

	disconnect(Core::instance(), SIGNAL(mainIconChanged(const QIcon &)),
		this, SLOT(statusPixmapChanged(const QIcon &)));
	disconnect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(pendingMessageDeleted()));

//	disconnect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	disconnect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	delete DockMenu;

	delete icon_timer;
	icon_timer = NULL;
	kdebugf2();
}

void DockingManager::changeIcon()
{
	kdebugf();
	if (pending.pendingMsgs() && !icon_timer->isActive())
	{
		switch (newMessageIcon)
		{
			case AnimatedEnvelope:
				emit trayMovieChanged(icons_manager->iconPath("MessageAnim"));
				break;
			case StaticEnvelope:
				emit trayPixmapChanged(icons_manager->loadIcon("Message"));
				break;
			case BlinkingEnvelope:
				if (!blink)
				{
					emit trayPixmapChanged(icons_manager->loadIcon("Message"));
					icon_timer->setSingleShot(true);
					icon_timer->start(500);
					blink = true;
				}
				else
				{
					Account *account = AccountManager::instance()->defaultAccount();
					if (!account || !account->protocol())
						return;

					const Status &stat = account->protocol()->status();
					emit trayPixmapChanged(QIcon(account->protocol()->statusPixmap(stat)));
					icon_timer->setSingleShot(true);
					icon_timer->start(500);
					blink = false;
				}
				break;
		}
	}
	else
		kdebugmf(KDEBUG_INFO, "OFF\n");
	kdebugf2();
}

void DockingManager::pendingMessageAdded()
{
	changeIcon();
}

void DockingManager::pendingMessageDeleted()
{
	if (!pending.pendingMsgs())
	{
		Account *account = AccountManager::instance()->defaultAccount();
		if (!account || !account->protocol())
			return;

		const Status &stat = account->protocol()->status();
		emit trayPixmapChanged(QIcon(account->protocol()->statusPixmap(stat)));
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

		emit trayTooltipChanged(tiptext);
	}
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == Qt::MidButton)
	{
		emit mousePressMidButton();
		return;
	}

	if (e->button() == Qt::LeftButton)
	{
		QWidget *kadu = Core::instance()->kaduWindow();

		emit mousePressLeftButton();
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());

		if (pending.pendingMsgs() && (e->modifiers() != Qt::ControlModifier))
		{
			pending.openMessages();
			return;
		}

		if (kadu->isMinimized())
		{
			kadu->showNormal();
			activateWindow(kadu->winId());
			return;
		}
		else if (kadu->isVisible())
			kadu->hide();
		else
		{
			kadu->show();
			kadu->raise();

			activateWindow(kadu->winId());
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
	emit trayPixmapChanged(icon);
	defaultToolTip();
	changeIcon();
}

QIcon DockingManager::defaultPixmap()
{
	Account *account = AccountManager::instance()->defaultAccount();
	if (!account || !account->protocol())
	{
		return QIcon();
	}
	return QIcon(account->protocol()->statusPixmap(account->protocol()->status()));
}

void DockingManager::setDocked(bool docked)
{
	kdebugf();
	if (docked)
	{
		changeIcon();
		defaultToolTip();
		if (config_file.readBoolEntry("General", "RunDocked"))
			Core::instance()->setShowMainWindowOnStart(false);
	}
	else
	{
// 		kdebugm(KDEBUG_INFO, "closing: %d\n", Kadu::closing());
		Core::instance()->kaduWindow()->show(); // isClosing? TODO: 0.6.6
	}
	Core::instance()->kaduWindow()->setDocked(docked);
	kdebugf2();
}

void DockingManager::createDefaultConfiguration()
{
	config_file.addVariable("General", "RunDocked", false);
	config_file.addVariable("General", "ShowTooltipInTray", true);
	config_file.addVariable("Look", "NewMessageIcon", 0);
}

DockingManager* docking_manager = NULL;

/** @} */

