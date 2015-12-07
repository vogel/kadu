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

#include "docking-configuration-provider.h"

#include "docking-configuration.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

DockingConfigurationProvider::DockingConfigurationProvider(QObject *parent) :
		QObject{parent}
{
	createDefaultConfiguration();
	configurationUpdated();
}

DockingConfigurationProvider::~DockingConfigurationProvider()
{
}

void DockingConfigurationProvider::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "RunDocked", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowTooltipInTray", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NewMessageIcon", 0);
}

const DockingConfiguration & DockingConfigurationProvider::configuration() const
{
	return m_configuration;
}

void DockingConfigurationProvider::configurationUpdated()
{
	m_configuration = DockingConfiguration
	{
		RunDocked: Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "RunDocked"),
		ShowTooltipInTray: Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ShowTooltipInTray"),
		NewMessageIcon: static_cast<StatusNotifierItemAttentionMode>(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "NewMessageIcon"))
	};

	emit updated();
}

#include "moc_docking-configuration-provider.cpp"
