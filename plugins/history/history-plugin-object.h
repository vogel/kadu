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

#pragma once

#include "history-exports.h"

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyAdditionalDataDeleteHandlerManager;
class BuddyConfigurationWidgetFactoryRepository;
class BuddyHistoryDeleteHandler;
class ChatConfigurationWidgetFactoryRepository;
class HistoryBuddyConfigurationWidgetFactory;
class HistoryChatConfigurationWidgetFactory;
class History;
class PathsProvider;

class HISTORYAPI HistoryPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit HistoryPluginObject(QObject *parent = nullptr);
	virtual ~HistoryPluginObject();

	virtual void done() override;

	History * history() const;

private:
	QPointer<BuddyAdditionalDataDeleteHandlerManager> m_buddyAdditionalDataDeleteHandlerManager;
	QPointer<BuddyConfigurationWidgetFactoryRepository> m_buddyConfigurationWidgetFactoryRepository;
	QPointer<BuddyHistoryDeleteHandler> m_buddyHistoryDeleteHandler;
	QPointer<ChatConfigurationWidgetFactoryRepository> m_chatConfigurationWidgetFactoryRepository;
	QPointer<HistoryBuddyConfigurationWidgetFactory> m_historyBuddyConfigurationWidgetFactory;
	QPointer<HistoryChatConfigurationWidgetFactory> m_historyChatConfigurationWidgetFactory;
	QPointer<History> m_history;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_INIT void init();
	INJEQT_SETTER void setBuddyAdditionalDataDeleteHandlerManager(BuddyAdditionalDataDeleteHandlerManager *buddyAdditionalDataDeleteHandlerManager);
	INJEQT_SETTER void setBuddyConfigurationWidgetFactoryRepository(BuddyConfigurationWidgetFactoryRepository *buddyConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setBuddyHistoryDeleteHandler(BuddyHistoryDeleteHandler *buddyHistoryDeleteHandler);
	INJEQT_SETTER void setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository);
	INJEQT_SETTER void setHistoryBuddyConfigurationWidgetFactory(HistoryBuddyConfigurationWidgetFactory *historyBuddyConfigurationWidgetFactory);
	INJEQT_SETTER void setHistoryChatConfigurationWidgetFactory(HistoryChatConfigurationWidgetFactory *historyChatConfigurationWidgetFactory);
	INJEQT_SETTER void setHistory(History *history);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
