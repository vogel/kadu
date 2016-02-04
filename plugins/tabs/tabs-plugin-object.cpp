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

#include "tabs-plugin-object.h"

#include "tabs-chat-widget-container-handler.h"
#include "tabs.h"

#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

TabsPluginObject::TabsPluginObject(QObject *parent) :
		QObject{parent}
{
}

TabsPluginObject::~TabsPluginObject()
{
}

void TabsPluginObject::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;
}

void TabsPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void TabsPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void TabsPluginObject::setTabsChatWidgetContainerHandler(TabsChatWidgetContainerHandler *tabsChatWidgetContainerHandler)
{
	m_tabsChatWidgetContainerHandler = tabsChatWidgetContainerHandler;
}

void TabsPluginObject::setTabsManager(TabsManager *tabsManager)
{
	m_tabsManager = tabsManager;
}

void TabsPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/tabs.ui"));
	m_chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(m_tabsChatWidgetContainerHandler);
	m_tabsManager->openStoredChatTabs();
}

void TabsPluginObject::done()
{
	m_tabsManager->storeOpenedChatTabs();
	m_chatWidgetContainerHandlerRepository->unregisterChatWidgetContainerHandler(m_tabsChatWidgetContainerHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/tabs.ui"));
}

#include "moc_tabs-plugin-object.cpp"
