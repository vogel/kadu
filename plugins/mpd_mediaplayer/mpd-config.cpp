/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "widgets/configuration/configuration-widget.h"
#include "misc/paths-provider.h"

#include "mpd-config.h"

MPDConfig::MPDConfig()
{
}

MPDConfig::MPDConfig(Configuration *configuration) :
		m_configuration{configuration}
{
	createDefaultConfiguration();
	Host = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDHost");
	Port = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDPort");
	Timeout = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDTimeout");
}

MPDConfig::~MPDConfig()
{
}

void MPDConfig::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("MediaPlayer", "MPDHost", "localhost");
	m_configuration->deprecatedApi()->addVariable("MediaPlayer", "MPDPort", "6600");
	m_configuration->deprecatedApi()->addVariable("MediaPlayer", "MPDTimeout", "10");
}

void MPDConfig::configurationUpdated()
{
	Host = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDHost");
	Port = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDPort");
	Timeout = m_configuration->deprecatedApi()->readEntry("MediaPlayer", "MPDTimeout");
}
