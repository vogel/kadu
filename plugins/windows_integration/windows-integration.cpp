/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "windows-integration.h"

#include "windows-taskbar-progress.h"
#include "windows-thumbnail-toolbar.h"

#include "file-transfer/file-transfer-manager.h"
#include "windows/main-window-repository.h"
#include "misc/memory.h"
#include "plugin/plugin-injected-factory.h"
#include "status/status-actions.h"
#include "status/status-container-manager.h"

#include <QtWidgets/QWidget>

WindowsIntegration::WindowsIntegration(QObject *parent) :
	QObject{parent}
{
}

WindowsIntegration::~WindowsIntegration()
{
}

void WindowsIntegration::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
}

void WindowsIntegration::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void WindowsIntegration::setMainWindowRepository(MainWindowRepository *mainWindowRepository)
{
	m_mainWindowRepository = mainWindowRepository;
	connect(m_mainWindowRepository, &MainWindowRepository::mainWindowAdded, this, &WindowsIntegration::mainWindowAdded);
}

void WindowsIntegration::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void WindowsIntegration::init()
{
	for (auto mainWindow : *m_mainWindowRepository)
		mainWindowAdded(mainWindow);
}

void WindowsIntegration::mainWindowAdded(QWidget *mainWindow)
{
	make_owned<WindowsTaskbarProgress>(m_fileTransferManager, mainWindow->window());

	auto statusActions = m_pluginInjectedFactory->makeNotOwned<StatusActions>(m_statusContainerManager, false, true, nullptr);
	m_pluginInjectedFactory->makeOwned<WindowsThumbnailToolbar>(std::move(statusActions), mainWindow->window());
}

#include "moc_windows-integration.cpp"
