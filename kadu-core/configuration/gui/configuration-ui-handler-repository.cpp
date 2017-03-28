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

#include "configuration-ui-handler-repository.h"

ConfigurationUiHandlerRepository::ConfigurationUiHandlerRepository(QObject *parent) : QObject{parent}
{
}

ConfigurationUiHandlerRepository::~ConfigurationUiHandlerRepository()
{
}

void ConfigurationUiHandlerRepository::addConfigurationUiHandler(ConfigurationUiHandler *configurationUiHandler)
{
    auto found = std::find(begin(), end(), configurationUiHandler);
    if (found == end())
    {
        m_configuratorUiHandlers.push_back(configurationUiHandler);
        emit configurationUiHandlerAdded(configurationUiHandler);
    }
}

void ConfigurationUiHandlerRepository::removeConfigurationUiHandler(ConfigurationUiHandler *configurationUiHandler)
{
    auto found = std::find(begin(), end(), configurationUiHandler);
    if (found != end())
    {
        m_configuratorUiHandlers.erase(found);
        emit configurationUiHandlerRemoved(configurationUiHandler);
    }
}
