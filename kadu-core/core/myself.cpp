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

#include "myself.h"

#include "buddies/buddy-shared.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injector-provider.h"
#include "misc/memory.h"

Myself::Myself(QObject *parent) : QObject{parent}
{
}

Myself::~Myself()
{
}

void Myself::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void Myself::setInjectorProvider(InjectorProvider *injectorProvider)
{
    m_injectorProvider = injectorProvider;
}

void Myself::makeMyself()
{
    m_buddy = Buddy{new BuddyShared{}};
    m_buddy.setAnonymous(false);
    configurationUpdated();
    m_injectorProvider->injector().inject_into(m_buddy.data());
}

void Myself::configurationUpdated()
{
    m_buddy.setDisplay(m_configuration->deprecatedApi()->readEntry("General", "Nick", tr("Me")));
}

Buddy Myself::buddy()
{
    if (!m_buddy)
        makeMyself();
    return m_buddy;
}
