/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-configuration-holder.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "misc/paths-provider.h"

StatusConfigurationHolder::StatusConfigurationHolder(QObject *parent) :
		ConfigurationHolder{parent},
		SetStatus{SetStatusPerIdentity}
{
}

StatusConfigurationHolder::~StatusConfigurationHolder()
{
}

void StatusConfigurationHolder::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void StatusConfigurationHolder::init()
{
	configurationUpdated();
}

void StatusConfigurationHolder::configurationUpdated()
{
	auto statusContainerType = m_configuration->deprecatedApi()->readEntry("General", "StatusContainerType", "Identity");

	auto newStatus = SetStatusPerIdentity;
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

#include "moc_status-configuration-holder.cpp"
