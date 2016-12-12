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

#include "screenshot-plugin-object.h"

#include "configuration/gui/screenshot-configuration-ui-handler.h"
#include "configuration/screen-shot-configuration.h"
#include "gui/actions/screenshot-actions.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"
#include "misc/paths-provider.h"

ScreenshotPluginObject::ScreenshotPluginObject(QObject *parent) :
		QObject{parent}
{
}

ScreenshotPluginObject::~ScreenshotPluginObject()
{
}

void ScreenshotPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void ScreenshotPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void ScreenshotPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void ScreenshotPluginObject::setScreenshotActions(ScreenshotActions *screenshotActions)
{
	m_screenshotActions = screenshotActions;
}

void ScreenshotPluginObject::setScreenShotConfigurationUiHandler(ScreenShotConfigurationUiHandler *screenShotConfigurationUiHandler)
{
	m_screenShotConfigurationUiHandler = screenShotConfigurationUiHandler;
}

void ScreenshotPluginObject::setScreenShotConfiguration(ScreenShotConfiguration *screenShotConfiguration)
{
	m_screenShotConfiguration = screenShotConfiguration;
}

void ScreenshotPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/screenshot.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_screenShotConfigurationUiHandler);
}

void ScreenshotPluginObject::done()
{
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_screenShotConfigurationUiHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/screenshot.ui"));
}

#include "moc_screenshot-plugin-object.cpp"
