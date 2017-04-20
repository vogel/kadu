/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification-configuration.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

NotificationConfiguration::NotificationConfiguration(QObject *parent)
        : QObject{parent}, m_newMessageOnlyIfInactive{true}, m_notifyIgnoreOnConnection{true}, m_ignoreOnlineToOnline{
                                                                                                   true}
{
}

NotificationConfiguration::~NotificationConfiguration()
{
}

void NotificationConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void NotificationConfiguration::init()
{
    configurationUpdated();
}

void NotificationConfiguration::configurationUpdated()
{
    m_newMessageOnlyIfInactive = m_configuration->deprecatedApi()->readBoolEntry("Notify", "NewMessageOnlyIfInactive");
    m_notifyIgnoreOnConnection = m_configuration->deprecatedApi()->readBoolEntry("Notify", "NotifyIgnoreOnConnection");
    m_ignoreOnlineToOnline = m_configuration->deprecatedApi()->readBoolEntry("Notify", "IgnoreOnlineToOnline");
}

QString NotificationConfiguration::notifyConfigurationKey(const QString &eventType) const
{
    auto event = eventType;
    while (true)
    {
        int slashPosition = event.lastIndexOf('/');
        if (-1 == slashPosition)
            return event;

        if (m_configuration->deprecatedApi()->readBoolEntry("Notify", event + "_UseCustomSettings", false))
            return event;

        event = event.left(slashPosition);
    }

    Q_ASSERT(false);
}
