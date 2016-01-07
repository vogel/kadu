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

#include "autostatus-plugin-object.h"

#include "autostatus-actions.h"
#include "autostatus-service.h"
#include "autostatus-status-changer.h"

#include "gui/windows/main-configuration-window.h"
#include "kadu/kadu-core/misc/paths-provider.h"
#include "status/status-changer-manager.h"

AutostatusPluginObject::AutostatusPluginObject(QObject *parent) :
		QObject{parent}
{
}

AutostatusPluginObject::~AutostatusPluginObject()
{
}

void AutostatusPluginObject::setAutostatusActions(AutostatusActions *autostatusActions)
{
	m_autostatusActions = autostatusActions;
}

void AutostatusPluginObject::setAutostatusService(AutostatusService *autostatusService)
{
	m_autostatusService = autostatusService;
}

void AutostatusPluginObject::setAutostatusStatusChanger(AutostatusStatusChanger *autostatusStatusChanger)
{
	m_autostatusStatusChanger = autostatusStatusChanger;
}

void AutostatusPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AutostatusPluginObject::setStatusChangerManager(StatusChangerManager *statusChangerManager)
{
	m_statusChangerManager = statusChangerManager;
}

void AutostatusPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autostatus.ui"));
	m_statusChangerManager->registerStatusChanger(m_autostatusStatusChanger);
}

void AutostatusPluginObject::done()
{
	m_statusChangerManager->unregisterStatusChanger(m_autostatusStatusChanger);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autostatus.ui"));
}

#include "moc_autostatus-plugin-object.cpp"
