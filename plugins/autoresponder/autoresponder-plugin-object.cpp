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

#include "autoresponder-plugin-object.h"

#include "autoresponder-configuration-ui-handler.h"
#include "autoresponder-message-filter.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "message/message-filter-service.h"

AutoresponderPluginObject::AutoresponderPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

AutoresponderPluginObject::~AutoresponderPluginObject()
{
}

void AutoresponderPluginObject::setAutoresponderConfigurationUiHandler(AutoresponderConfigurationUiHandler *autoresponderConfigurationUiHandler)
{
	m_autoresponderConfigurationUiHandler = autoresponderConfigurationUiHandler;
}

void AutoresponderPluginObject::setAutoresponderMessageFilter(AutoresponderMessageFilter *autoresponderMessageFilter)
{
	m_autoresponderMessageFilter = autoresponderMessageFilter;
}

void AutoresponderPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void AutoresponderPluginObject::setMessageFilterService(MessageFilterService *messageFilterService)
{
	m_messageFilterService = messageFilterService;
}

void AutoresponderPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void AutoresponderPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_autoresponderConfigurationUiHandler);
	m_messageFilterService->registerMessageFilter(m_autoresponderMessageFilter);
}

void AutoresponderPluginObject::done()
{
	m_messageFilterService->unregisterMessageFilter(m_autoresponderMessageFilter);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_autoresponderConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/autoresponder.ui"));
}

#include "moc_autoresponder-plugin-object.cpp"
