/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"

#include "main-configuration-holder.h"

MainConfigurationHolder *MainConfigurationHolder::Instance = 0;

MainConfigurationHolder * MainConfigurationHolder::instance()
{
	return Instance;
}

void MainConfigurationHolder::createInstance()
{
	if (!Instance)
		Instance = new MainConfigurationHolder();
}

void MainConfigurationHolder::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

MainConfigurationHolder::MainConfigurationHolder() :
		SetStatus(SetStatusPerIdentity)
{
	configurationUpdated();
}

void MainConfigurationHolder::configurationUpdated()
{
	QString statusContainerType = config_file.readEntry("General", "StatusContainerType", "Identity");

	SetStatusMode newStatus = SetStatusPerIdentity;
	if (statusContainerType == "Account")
		newStatus = SetStatusPerAccount;
	else if (statusContainerType == "All")
		newStatus = SetStatusForAll;

	if (SetStatus != newStatus)
	{
		SetStatus = newStatus;
		emit setStatusModeChanged();
	}
}
