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

#include "exports.h"

#include <QtCore/QObject>
#include <vector>

class ConfigurationUiHandler;

class KADUAPI ConfigurationUiHandlerRepository : public QObject
{
    Q_OBJECT

    using Storage = std::vector<ConfigurationUiHandler *>;
    using WrappedIterator = Storage::iterator;

public:
    using Iterator = Storage::iterator;

    Q_INVOKABLE explicit ConfigurationUiHandlerRepository(QObject *parent = nullptr);
    virtual ~ConfigurationUiHandlerRepository();

    Iterator begin()
    {
        return std::begin(m_configuratorUiHandlers);
    }
    Iterator end()
    {
        return std::end(m_configuratorUiHandlers);
    }

    void addConfigurationUiHandler(ConfigurationUiHandler *configurationUiHandler);
    void removeConfigurationUiHandler(ConfigurationUiHandler *configurationUiHandler);

signals:
    void configurationUiHandlerAdded(ConfigurationUiHandler *configurationUiHandler);
    void configurationUiHandlerRemoved(ConfigurationUiHandler *configurationUiHandler);

private:
    Storage m_configuratorUiHandlers;
};

inline ConfigurationUiHandlerRepository::Iterator
begin(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
    return configurationUiHandlerRepository->begin();
}

inline ConfigurationUiHandlerRepository::Iterator
end(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
    return configurationUiHandlerRepository->end();
}
