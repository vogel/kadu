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

#include "history-plugin-object.h"

#include "gui/widgets/history-buddy-configuration-widget-factory.h"
#include "gui/widgets/history-chat-configuration-widget-factory.h"
#include "buddy-history-delete-handler.h"
#include "history.h"

#include "buddies/buddy-additional-data-delete-handler-manager.h"
#include "misc/paths-provider.h"
#include "widgets/buddy-configuration-widget-factory-repository.h"
#include "widgets/chat-configuration-widget-factory-repository.h"
#include "windows/main-configuration-window-service.h"
#include "windows/main-configuration-window.h"

HistoryPluginObject::HistoryPluginObject(QObject *parent) :
		QObject{parent}
{
}

HistoryPluginObject::~HistoryPluginObject()
{
}

void HistoryPluginObject::setBuddyAdditionalDataDeleteHandlerManager(BuddyAdditionalDataDeleteHandlerManager *buddyAdditionalDataDeleteHandlerManager)
{
	m_buddyAdditionalDataDeleteHandlerManager = buddyAdditionalDataDeleteHandlerManager;
}

void HistoryPluginObject::setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository)
{
	m_buddyConfigurationWidgetFactoryRepository = buddyConfigurationWidgetFactoryRepository;
}

void HistoryPluginObject::setBuddyHistoryDeleteHandler(BuddyHistoryDeleteHandler *buddyHistoryDeleteHandler)
{
	m_buddyHistoryDeleteHandler = buddyHistoryDeleteHandler;
}

void HistoryPluginObject::setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository)
{
	m_chatConfigurationWidgetFactoryRepository = chatConfigurationWidgetFactoryRepository;
}

void HistoryPluginObject::setHistoryBuddyConfigurationWidgetFactory(HistoryBuddyConfigurationWidgetFactory *historyBuddyConfigurationWidgetFactory)
{
	m_historyBuddyConfigurationWidgetFactory = historyBuddyConfigurationWidgetFactory;
}

void HistoryPluginObject::setHistoryChatConfigurationWidgetFactory(HistoryChatConfigurationWidgetFactory *historyChatConfigurationWidgetFactory)
{
	m_historyChatConfigurationWidgetFactory = historyChatConfigurationWidgetFactory;
}

void HistoryPluginObject::setHistory(History *history)
{
	m_history = history;
}

void HistoryPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void HistoryPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

History * HistoryPluginObject::history() const
{
	return m_history;
}

void HistoryPluginObject::init()
{
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/history.ui"));
	m_buddyAdditionalDataDeleteHandlerManager->registerAdditionalDataDeleteHandler(m_buddyHistoryDeleteHandler);
	m_buddyConfigurationWidgetFactoryRepository->registerFactory(m_historyBuddyConfigurationWidgetFactory);
	m_chatConfigurationWidgetFactoryRepository->registerFactory(m_historyChatConfigurationWidgetFactory);
}

void HistoryPluginObject::done()
{
	m_chatConfigurationWidgetFactoryRepository->unregisterFactory(m_historyChatConfigurationWidgetFactory);
	m_buddyConfigurationWidgetFactoryRepository->unregisterFactory(m_historyBuddyConfigurationWidgetFactory);
	m_buddyAdditionalDataDeleteHandlerManager->unregisterAdditionalDataDeleteHandler(m_buddyHistoryDeleteHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/history.ui"));
}

#include "moc_history-plugin-object.cpp"
