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

#include "auto-hide-plugin-object.h"

#include "auto-hide.h"
#include "auto-hide-configuration-ui-handler.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"
#include "misc/paths-provider.h"

AutoHidePluginObject::AutoHidePluginObject(QObject *parent) :
		QObject{parent}
{
}

AutoHidePluginObject::~AutoHidePluginObject()
{
}

void AutoHidePluginObject::setAutoHide(AutoHide *autoHide)
{
	m_autoHide = autoHide;
}

void AutoHidePluginObject::setAutoHideConfigurationUiHandler(AutoHideConfigurationUiHandler *autoHideConfigurationUiHandler)
{
	m_autoHideConfigurationUiHandler = autoHideConfigurationUiHandler;
}

void AutoHidePluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void AutoHidePluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void AutoHidePluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AutoHidePluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/auto_hide.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_autoHideConfigurationUiHandler);
}

void AutoHidePluginObject::done()
{
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_autoHideConfigurationUiHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/auto_hide.ui"));
}

#include "moc_auto-hide-plugin-object.cpp"
