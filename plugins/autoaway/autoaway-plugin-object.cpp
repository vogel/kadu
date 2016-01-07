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

#include "autoaway-plugin-object.h"

#include "autoaway.h"
#include "autoaway-configuration-ui-handler.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "kadu/kadu-core/misc/paths-provider.h"
#include "status/status-changer-manager.h"

AutoawayPluginObject::AutoawayPluginObject(QObject *parent) :
		QObject{parent}
{
}

AutoawayPluginObject::~AutoawayPluginObject()
{
}

void AutoawayPluginObject::setAutoaway(Autoaway *autoaway)
{
	m_autoaway = autoaway;
}

void AutoawayPluginObject::setAutoawayConfigurationUiHandler(AutoawayConfigurationUiHandler *autoawayConfigurationUiHandler)
{
	m_autoawayConfigurationUiHandler = autoawayConfigurationUiHandler;
}

void AutoawayPluginObject::setAutoawayStatusChanger(AutoawayStatusChanger *autoawayStatusChanger)
{
	m_autoawayStatusChanger = autoawayStatusChanger;
}

void AutoawayPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void AutoawayPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AutoawayPluginObject::setStatusChangerManager(StatusChangerManager *statusChangerManager)
{
	m_statusChangerManager = statusChangerManager;
}

void AutoawayPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_autoawayConfigurationUiHandler);
	m_statusChangerManager->registerStatusChanger(m_autoawayStatusChanger);
}

void AutoawayPluginObject::done()
{
	m_statusChangerManager->unregisterStatusChanger(m_autoawayStatusChanger);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_autoawayConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));
}

#include "moc_autoaway-plugin-object.cpp"
