/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/buddy-configuration-widget-factory-repository.h"
#include "gui/widgets/chat-configuration-widget-factory-repository.h"
#include "misc/paths-provider.h"

#include "gui/widgets/history-buddy-configuration-widget-factory.h"
#include "gui/widgets/history-chat-configuration-widget-factory.h"
#include "gui/windows/history-window.h"
#include "buddy-history-delete-handler.h"
#include "history.h"

#include "history-plugin.h"

HistoryPlugin::HistoryPlugin() :
		MyBuddyConfigurationWidgetFactory(0), MyChatConfigurationWidgetFactory(0)
{
}

HistoryPlugin::~HistoryPlugin()
{
}

bool HistoryPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	History::createInstance();
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/history.ui"));
	MainConfigurationWindow::registerUiHandler(History::instance());

	BuddyHistoryDeleteHandler::createInstance();
	BuddyAdditionalDataDeleteHandlerManager::instance()->registerAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());

	registerServices();

	return true;
}

void HistoryPlugin::done()
{
	unregisterServices();

	BuddyAdditionalDataDeleteHandlerManager::instance()->unregisterAdditionalDataDeleteHandler(BuddyHistoryDeleteHandler::instance());
	BuddyHistoryDeleteHandler::destroyInstance();

	if (HistoryWindow::instance())
		delete HistoryWindow::instance();

	MainConfigurationWindow::unregisterUiHandler(History::instance());
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/history.ui"));
	History::destroyInstance();
}

void HistoryPlugin::registerServices()
{
	MyBuddyConfigurationWidgetFactory = new HistoryBuddyConfigurationWidgetFactory();
	MyChatConfigurationWidgetFactory = new HistoryChatConfigurationWidgetFactory();

	Core::instance()->buddyConfigurationWidgetFactoryRepository()->registerFactory(MyBuddyConfigurationWidgetFactory);
	Core::instance()->chatConfigurationWidgetFactoryRepository()->registerFactory(MyChatConfigurationWidgetFactory);
}

void HistoryPlugin::unregisterServices()
{
	Core::instance()->chatConfigurationWidgetFactoryRepository()->unregisterFactory(MyChatConfigurationWidgetFactory);
	Core::instance()->buddyConfigurationWidgetFactoryRepository()->unregisterFactory(MyBuddyConfigurationWidgetFactory);

	delete MyChatConfigurationWidgetFactory;
	MyChatConfigurationWidgetFactory = 0;
	delete MyBuddyConfigurationWidgetFactory;
	MyBuddyConfigurationWidgetFactory = 0;
}

Q_EXPORT_PLUGIN2(history, HistoryPlugin)

#include "moc_history-plugin.cpp"
