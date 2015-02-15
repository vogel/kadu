/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "tabs-chat-widget-container-handler.h"
#include "tabs.h"

#include "tabs-plugin.h"

TabsPlugin::TabsPlugin(QObject *parent) :
		QObject{parent},
		TabsManagerInstance{}
{
}


TabsPlugin::~TabsPlugin()
{
}

bool TabsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ChatWidgetContainerHandler.reset(new TabsChatWidgetContainerHandler());
	TabsManagerInstance = new TabsManager(this);
	TabsManagerInstance->setChatWidgetRepository(Core::instance()->chatWidgetRepository());
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/tabs.ui"));
	MainConfigurationWindow::registerUiHandler(TabsManagerInstance);

	ChatWidgetContainerHandler.data()->setTabsManager(TabsManagerInstance);
	ChatWidgetContainerHandler.data()->setTabWidget(TabsManagerInstance->tabWidget());
	Core::instance()->chatWidgetContainerHandlerRepository()->registerChatWidgetContainerHandler(ChatWidgetContainerHandler.data());

	TabsManagerInstance->openStoredChatTabs();

	return true;
}

void TabsPlugin::done()
{
	TabsManagerInstance->storeOpenedChatTabs();

	Core::instance()->chatWidgetContainerHandlerRepository()->unregisterChatWidgetContainerHandler(ChatWidgetContainerHandler.data());

	MainConfigurationWindow::unregisterUiHandler(TabsManagerInstance);
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/tabs.ui"));
	delete TabsManagerInstance;
	TabsManagerInstance = 0;
	ChatWidgetContainerHandler.reset();
}

Q_EXPORT_PLUGIN2(tabs, TabsPlugin)

#include "moc_tabs-plugin.cpp"
