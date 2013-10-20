/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>

#include "configuration/configuration-file.h"
#include "gui/widgets/group-tab-bar/group-tab-bar.h"
#include "gui/widgets/group-tab-bar/group-tab-bar-configuration.h"

#include "group-tab-bar-configurator.h"

void GroupTabBarConfigurator::setGroupTabBar(GroupTabBar *groupTabBar)
{
	ConfigurableGroupTabBar = groupTabBar;

	createDefaultConfiguration();
	configurationUpdated();
}

void GroupTabBarConfigurator::createDefaultConfiguration()
{
	config_file.addVariable("Look", "ShowGroupTabEverybody", true);
	config_file.addVariable("Look", "ShowGroupTabUngroupped", false);
	config_file.addVariable("Look", "DisplayGroupTabs", true);
}

void GroupTabBarConfigurator::configurationUpdated()
{
	if (!ConfigurableGroupTabBar)
		return;

	auto configuration = GroupTabBarConfiguration();

	configuration.setDisplayGroupTabs(config_file.readBoolEntry("Look", "DisplayGroupTabs", true));
	configuration.setShowGroupTabEverybody(config_file.readBoolEntry("Look", "ShowGroupTabEverybody", true));
	configuration.setShowGroupTabUngroupped(config_file.readBoolEntry("Look", "ShowGroupTabUngroupped", true));

	ConfigurableGroupTabBar.data()->setConfiguration(configuration);
}
