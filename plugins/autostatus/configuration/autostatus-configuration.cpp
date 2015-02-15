/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "core/application.h"
#include "misc/paths-provider.h"

#include "autostatus-configuration.h"

AutostatusConfiguration::AutostatusConfiguration()
{
	configurationUpdated();
}

void AutostatusConfiguration::configurationUpdated()
{
	AutoTime = Application::instance()->configuration()->deprecatedApi()->readNumEntry("PowerKadu", "autostatus_time", 10);
	AutoStatus = Application::instance()->configuration()->deprecatedApi()->readNumEntry("PowerKadu", "autoStatus");
	StatusFilePath = Application::instance()->configuration()->deprecatedApi()->readEntry("PowerKadu", "status_file_path", Application::instance()->pathsProvider()->profilePath() + QLatin1String("autostatus.list"));
}
