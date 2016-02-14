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
#include "status-notifier-item.h"

#include "core/injected-factory.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "icons/icons-manager.h"
#include "notification/silent-mode-service.h"
#include "status/status-container-manager.h"
#include "activate.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

DockingMenuHandler::DockingMenuHandler(QObject *parent) :
		QObject{parent},
#if defined(Q_OS_UNIX)
		m_showKaduAction{nullptr},
		m_hideKaduAction{nullptr},
#endif
		m_silentModeAction{nullptr},
		m_closeKaduAction{nullptr},
		m_needsUpdate{true},
		m_mainWindowLastVisible{true}
{
}

DockingMenuHandler::~DockingMenuHandler()
{
}

void DockingMenuHandler::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void DockingMenuHandler::setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository)
{
	m_dockingMenuActionRepository = dockingMenuActionRepository;
}

void DockingMenuHandler::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void DockingMenuHandler::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void DockingMenuHandler::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void DockingMenuHandler::setSilentModeService(SilentModeService *silentModeService)
{
	m_silentModeService = silentModeService;
}

void DockingMenuHandler::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void DockingMenuHandler::setStatusNotifierItem(StatusNotifierItem *statusNotifierItem)
{
	m_menu = statusNotifierItem->contextMenu();
}

void DockingMenuHandler::init()
{
	connect(m_iconsManager, SIGNAL(themeChanged()), this, SLOT(update()));
	connect(m_dockingMenuActionRepository, SIGNAL(actionAdded(QAction*)), this, SLOT(update()));
	connect(m_dockingMenuActionRepository, SIGNAL(actionRemoved(QAction*)), this, SLOT(update()));

	for (auto statusContainer : m_statusContainerManager->statusContainers())
		connect(statusContainer, SIGNAL(statusUpdated(StatusContainer*)), this, SLOT(update()));
	
	connect(m_statusContainerManager, SIGNAL(statusContainerRegistered(StatusContainer*)),
	        this, SLOT(statusContainerRegistered(StatusContainer*)));
	connect(m_statusContainerManager, SIGNAL(statusContainerUnregistered(StatusContainer*)),
	        this, SLOT(statusContainerUnregistered(StatusContainer*)));

	connect(m_silentModeService, &SilentModeService::silentModeToggled, this, &DockingMenuHandler::update);

	m_menu->setSeparatorsCollapsible(true);
	connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
	
#if defined(Q_OS_UNIX)
	m_showKaduAction = new QAction{tr("&Restore"), this};
	connect(m_showKaduAction, SIGNAL(triggered()), this, SLOT(showKaduWindow()));

	m_hideKaduAction = new QAction{tr("&Minimize"), this};
	connect(m_hideKaduAction, SIGNAL(triggered()), this, SLOT(hideKaduWindow()));
#endif

	m_silentModeAction = new QAction{m_iconsManager->iconByPath(KaduIcon{"kadu_icons/enable-notifications"}), tr("Silent mode"), this};
	m_silentModeAction->setCheckable(true);
	connect(m_silentModeAction, SIGNAL(triggered(bool)), this, SLOT(silentModeToggled(bool)));

	m_closeKaduAction = new QAction{m_iconsManager->iconByPath(KaduIcon{"application-exit"}), tr("&Exit Kadu"), this};
	connect(m_closeKaduAction, SIGNAL(triggered()), qApp, SLOT(quit()));
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
	if (m_needsUpdate)
		doUpdate();
}

void DockingMenuHandler::doUpdate()
{
	m_menu->clear();
	addStatusContainerMenus();
	addActionRepositoryMenus();
	m_menu->addSeparator();
	m_silentModeAction->setChecked(m_silentModeService->isSilent());
	m_menu->addAction(m_silentModeAction);
	m_menu->addSeparator();
#if defined(Q_OS_UNIX)
	m_mainWindowLastVisible = m_kaduWindowService->kaduWindow()->window()->isVisible();
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
		auto allAccountsMenu = m_injectedFactory->makeInjected<StatusMenu>(m_statusContainerManager, multipleMenus, m_menu);
		connect(allAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(update()));
	}
}

void DockingMenuHandler::addStatusContainerMenu(StatusContainer *statusContainer)
{
	auto menu = new QMenu{statusContainer->statusContainerName(), m_menu};
	menu->setIcon(m_iconsManager->iconByPath(statusContainer->statusIcon()));
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
	_activateWindow(m_configuration, m_kaduWindowService->kaduWindow());
}

void DockingMenuHandler::hideKaduWindow()
{
	m_kaduWindowService->kaduWindow()->window()->hide();
}

void DockingMenuHandler::silentModeToggled(bool enabled)
{
	m_silentModeService->setSilent(enabled);
}

#include "moc_docking-menu-handler.cpp"
