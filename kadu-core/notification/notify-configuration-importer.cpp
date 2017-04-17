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

#include "notify-configuration-importer.h"
#include "notify-configuration-importer.moc"

#include "buddies/buddy-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

NotifyConfigurationImporter::NotifyConfigurationImporter(QObject *parent) : QObject{parent}
{
}

NotifyConfigurationImporter::~NotifyConfigurationImporter()
{
}

void NotifyConfigurationImporter::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void NotifyConfigurationImporter::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void NotifyConfigurationImporter::import()
{
    if (m_configuration->deprecatedApi()->readBoolEntry("General", "ImportedPre10"))
        return;

    for (auto const &buddy : m_buddyManager->items())
    {
        if (buddy.isNull() || buddy.isAnonymous())
            continue;

        auto notify = buddy.property("notify:Notify", false).toBool() ||
                      m_configuration->deprecatedApi()->readBoolEntry("Notify", "NotifyAboutAll");
        if (notify)
            buddy.removeProperty("notify:Notify");
        else
            buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
    }

    m_configuration->deprecatedApi()->addVariable("General", "ImportedPre10", true);
}
