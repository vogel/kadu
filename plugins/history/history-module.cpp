/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-module.h"

#include "actions/clear-history-action.h"
#include "actions/show-history-action.h"
#include "buddy-history-delete-handler.h"
#include "gui/widgets/history-buddy-configuration-widget-factory.h"
#include "gui/widgets/history-chat-configuration-widget-factory.h"
#include "gui/windows/history-window-service.h"
#include "history-plugin-object.h"
#include "history.h"

HistoryModule::HistoryModule()
{
    add_type<BuddyHistoryDeleteHandler>();
    add_type<ClearHistoryAction>();
    add_type<HistoryBuddyConfigurationWidgetFactory>();
    add_type<HistoryChatConfigurationWidgetFactory>();
    add_type<HistoryPluginObject>();
    add_type<HistoryWindowService>();
    add_type<History>();
    add_type<ShowHistoryAction>();
}

HistoryModule::~HistoryModule()
{
}
