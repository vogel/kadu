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

#include "buddies/buddy-storage.h"
#include "configuration/deprecated-configuration-api.h"

Myself::Myself(QObject *parent) :
		QObject{parent}
{
}

Myself::~Myself()
{
}

void Myself::setBuddyStorage(BuddyStorage *buddyStorage)
{
	m_buddyStorage = buddyStorage;
}

void Myself::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void Myself::init()
{
	m_buddy = m_buddyStorage->create();
	m_buddy.setAnonymous(false);
	configurationUpdated();
}

void Myself::configurationUpdated()
{
	m_buddy.setDisplay(m_configuration->deprecatedApi()->readEntry("General", "Nick", tr("Me")));
}

Buddy Myself::buddy()
{
	return m_buddy;
}
