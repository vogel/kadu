/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-buddy-configuration-widget-factory.h"

#include "gui/widgets/history-buddy-configuration-widget.h"
#include "plugin/plugin-injected-factory.h"

HistoryBuddyConfigurationWidgetFactory::HistoryBuddyConfigurationWidgetFactory(QObject *parent) : QObject{parent}
{
}

HistoryBuddyConfigurationWidgetFactory::~HistoryBuddyConfigurationWidgetFactory()
{
}

void HistoryBuddyConfigurationWidgetFactory::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

BuddyConfigurationWidget *HistoryBuddyConfigurationWidgetFactory::createWidget(const Buddy &buddy, QWidget *parent)
{
    return m_pluginInjectedFactory->makeInjected<HistoryBuddyConfigurationWidget>(buddy, parent);
}

#include "moc_history-buddy-configuration-widget-factory.cpp"
