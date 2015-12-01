/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-menu-handler.h"

#include "docking-menu-action-repository.h"

#include "core/core.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/kadu-window.h"
#include "icons/icons-manager.h"
#include "notification/notification-service.h"
#include "status/status-container-manager.h"
#include "activate.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

DockingMenuHandler::DockingMenuHandler(QMenu *menu, QObject *parent) :
		QObject{parent},
		m_menu{menu},
#if defined(Q_OS_UNIX)
		m_showKaduAction{nullptr},
		m_hideKaduAction{nullptr},
#endif
		m_silentModeAction{nullptr},
		m_closeKaduAction{nullptr},
		m_needsUpdate{true},
		m_mainWindowLastVisible{true}
{
	m_menu->setSeparatorsCollapsible(true);
	connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
	
#if defined(Q_OS_UNIX)
	m_showKaduAction = new QAction{tr("&Restore"), this};
	connect(m_showKaduAction, SIGNAL(triggered()), this, SLOT(showKaduWindow()));

	m_hideKaduAction = new QAction{tr("&Minimize"), this};
	connect(m_hideKaduAction, SIGNAL(triggered()), this, SLOT(hideKaduWindow()));
#endif

	m_silentModeAction = new QAction{KaduIcon{"kadu_icons/enable-notifications"}.icon(), tr("Silent mode"), this};
	m_silentModeAction->setCheckable(true);
	connect(m_silentModeAction, SIGNAL(triggered(bool)), this, SLOT(silentModeToggled(bool)));

	m_closeKaduAction = new QAction{KaduIcon{"application-exit"}.icon(), tr("&Exit Kadu"), this};
	connect(m_closeKaduAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

DockingMenuHandler::~DockingMenuHandler()
{
}

void DockingMenuHandler::setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository)
{
	m_dockingMenuActionRepository = dockingMenuActionRepository;
	connect(m_dockingMenuActionRepository, SIGNAL(actionAdded(QAction*)), this, SLOT(update()));
	connect(m_dockingMenuActionRepository, SIGNAL(actionRemoved(QAction*)), this, SLOT(update()));
}

void DockingMenuHandler::setIconsManager(IconsManager *iconsManager)
{
	connect(iconsManager, SIGNAL(themeChanged()), this, SLOT(update()));
}

void DockingMenuHandler::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
	connect(m_notificationService, SIGNAL(silentModeToggled(bool)), this, SLOT(update()));
}

void DockingMenuHandler::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
	for (auto statusContainer : m_statusContainerManager->statusContainers())
		connect(statusContainer, SIGNAL(statusUpdated(StatusContainer*)), this, SLOT(update()));
	
	connect(m_statusContainerManager, SIGNAL(statusContainerRegistered(StatusContainer*)),
	        this, SLOT(statusContainerRegistered(StatusContainer*)));
	connect(m_statusContainerManager, SIGNAL(statusContainerUnregistered(StatusContainer*)),
	        this, SLOT(statusContainerUnregistered(StatusContainer*)));
}

void DockingMenuHandler::statusContainerRegistered(StatusContainer *statusContainer)
{
	connect(statusContainer, SIGNAL(statusUpdated(StatusContainer*)), this, SLOT(update()));
	update();
}

void DockingMenuHandler::statusContainerUnregistered(StatusContainer *statusContainer)
{
	disconnect(statusContainer, SIGNAL(statusUpdated(StatusContainer*)), this, SLOT(update()));
	update();
}

void DockingMenuHandler::update()
{
	m_needsUpdate = true;
}

void DockingMenuHandler::aboutToShow()
{
	if (!m_needsUpdate
#if defined(Q_OS_UNIX)
			|| Core::instance()->kaduWindow()->window()->isVisible() != m_mainWindowLastVisible
#endif
	)
		return;

	doUpdate();
}

void DockingMenuHandler::doUpdate()
{
	m_menu->clear();
	addStatusContainerMenus();
	addActionRepositoryMenus();
	m_menu->addSeparator();
	m_silentModeAction->setChecked(m_notificationService->silentMode());
	m_menu->addAction(m_silentModeAction);
	m_menu->addSeparator();
#if defined(Q_OS_UNIX)
	m_mainWindowLastVisible = Core::instance()->kaduWindow()->window()->isVisible();
	m_menu->addAction(m_mainWindowLastVisible ? m_hideKaduAction : m_showKaduAction);
#endif
	m_menu->addAction(m_closeKaduAction);

	m_needsUpdate = false;
}

void DockingMenuHandler::addStatusContainerMenus()
{
	auto statusContainersCount = m_statusContainerManager->statusContainers().count();
	auto showAllAccountsMenu = statusContainersCount > 0;
	auto multipleMenus = statusContainersCount > 1;

	if (multipleMenus)
	{
		for (auto statusContainer : m_statusContainerManager->statusContainers())
			addStatusContainerMenu(statusContainer);
		m_menu->addSeparator();
	}
	
	if (showAllAccountsMenu)
	{
		auto allAccountsMenu = new StatusMenu{StatusContainerManager::instance(), multipleMenus, m_menu};
		connect(allAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(update()));
	}
}

void DockingMenuHandler::addStatusContainerMenu(StatusContainer *statusContainer)
{
	auto menu = new QMenu{statusContainer->statusContainerName(), m_menu};
	menu->setIcon(statusContainer->statusIcon().icon());
	new StatusMenu{statusContainer, false, menu};
	m_menu->addMenu(menu);
}

void DockingMenuHandler::addActionRepositoryMenus()
{
	if (!m_dockingMenuActionRepository->actions().isEmpty())
	{
		m_menu->addSeparator();
		for (auto action : m_dockingMenuActionRepository->actions())
			m_menu->addAction(action);
	}
}

void DockingMenuHandler::showKaduWindow()
{
	_activateWindow(Core::instance()->kaduWindow());
}

void DockingMenuHandler::hideKaduWindow()
{
	Core::instance()->kaduWindow()->window()->hide();
}

void DockingMenuHandler::silentModeToggled(bool enabled)
{
	m_notificationService->setSilentMode(enabled);
}

#include "moc_docking-menu-handler.cpp"
