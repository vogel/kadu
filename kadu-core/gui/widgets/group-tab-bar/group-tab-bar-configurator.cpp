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

#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/widgets/group-tab-bar/group-tab-bar.h"
#include "gui/widgets/group-tab-bar/group-tab-bar-configuration.h"

#include "group-tab-bar-configurator.h"

GroupTabBarConfigurator::GroupTabBarConfigurator()
{
	createDefaultConfiguration();
}

void GroupTabBarConfigurator::setGroupTabBar(GroupTabBar *groupTabBar)
{
	ConfigurableGroupTabBar = groupTabBar;
	if (ConfigurableGroupTabBar)
		ConfigurableGroupTabBar.data()->setInitialConfiguration(loadConfiguration());
}

void GroupTabBarConfigurator::createDefaultConfiguration()
{
	config_file.addVariable("Look", "ShowGroupAll", true);
	config_file.addVariable("Look", "ShowGroupTabUngroupped", false);
	config_file.addVariable("Look", "DisplayGroupTabs", true);
}

void GroupTabBarConfigurator::configurationUpdated()
{
	if (!ConfigurableGroupTabBar)
		return;

	ConfigurableGroupTabBar.data()->setConfiguration(loadConfiguration());
}

GroupTabBarConfiguration GroupTabBarConfigurator::loadConfiguration() const
{
	auto configuration = GroupTabBarConfiguration();

	configuration.setDisplayGroupTabs(config_file.readBoolEntry("Look", "DisplayGroupTabs", true));
	configuration.setShowGroupTabEverybody(config_file.readBoolEntry("Look", "ShowGroupAll", true));
	configuration.setAlwaysShowGroupTabUngroupped(config_file.readBoolEntry("Look", "AlwaysShowGroupTabUngroupped", true));
	configuration.setCurrentGroupTab(config_file.readNumEntry("Look", "CurrentGroupTab", 0));
	configuration.setGroupFilters(loadGroupFilters(configuration.showGroupTabEverybody()));

	return configuration;
}

void GroupTabBarConfigurator::storeConfiguration()
{
	if (!ConfigurableGroupTabBar)
		return;

	auto configuration = ConfigurableGroupTabBar.data()->configuration();
	config_file.writeEntry("Look", "CurrentGroupTab", configuration.currentGroupTab());

	storeGroupFilters(configuration.groupFilters());

	config_file.sync(); // TODO: fix whole configuration system
}

QVector<GroupFilter> GroupTabBarConfigurator::loadGroupFilters(bool showGroupTabEverybody) const
{
	auto groupTabBarNode = xml_config_file->getNode("GroupTabBar", XmlConfigFile::ModeGet);
	if (groupTabBarNode.isNull())
		return import_0_12_groupFilters(showGroupTabEverybody);

	auto result = QVector<GroupFilter>();
	auto groupFilterNodes = xml_config_file->getNodes(groupTabBarNode, "GroupFilter");
	foreach (const auto &groupFilterNode, groupFilterNodes)
	{
		auto groupFilter = loadGroupFilter(groupFilterNode);
		if (GroupFilterInvalid != groupFilter.filterType())
			result.append(groupFilter);
	}

	return result;
}

GroupFilter GroupTabBarConfigurator::loadGroupFilter(QDomElement element) const
{
	auto type = xml_config_file->getTextNode(element, "Type");
	auto groupUuid = xml_config_file->getTextNode(element, "Group");

	if (type == "Regular")
		return GroupFilter(GroupManager::instance()->byUuid(groupUuid));
	if (type == "Everybody")
		return GroupFilter(GroupFilterEverybody);
	if (type == "Ungroupped")
		return GroupFilter(GroupFilterUngroupped);
	return GroupFilter();
}

QVector<GroupFilter> GroupTabBarConfigurator::import_0_12_groupFilters(bool showGroupTabEverybody) const
{
	auto result = QVector<GroupFilter>();
	auto position = showGroupTabEverybody
			? config_file.readNumEntry("Look", "AllGroupTabPosition", 0)
			: config_file.readNumEntry("Look", "UngroupedGroupTabPosition", 0);

	auto groups = GroupManager::instance()->items().toList();
	qStableSort(groups.begin(), groups.end(), [](const Group &a, const Group &b){ return a.tabPosition() < b.tabPosition(); });

	foreach (const auto &group, groups)
		result.append(GroupFilter(group));

	result.insert(qBound(0, position, result.size()), GroupFilter(showGroupTabEverybody ? GroupFilterEverybody : GroupFilterUngroupped));

	return result;
}

void GroupTabBarConfigurator::storeGroupFilters(const QVector<GroupFilter> &groupFilters)
{
	auto groupTabBarNode = xml_config_file->getNode("GroupTabBar", XmlConfigFile::ModeCreate);
	foreach (const auto &groupFilter, groupFilters)
		storeGroupFilter(groupTabBarNode, groupFilter);
}

void GroupTabBarConfigurator::storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter)
{
	if (GroupFilterInvalid == groupFilter.filterType())
		return;

	auto groupFilterNode = xml_config_file->getNode(parentElement, "GroupFilter", XmlConfigFile::ModeAppend);
	switch (groupFilter.filterType())
	{
		case GroupFilterRegular:
			xml_config_file->createTextNode(groupFilterNode, "Type", "Regular");
			xml_config_file->createTextNode(groupFilterNode, "Group", groupFilter.group().uuid().toString());
			break;
		case GroupFilterEverybody:
			xml_config_file->createTextNode(groupFilterNode, "Type", "Everybody");
			break;
		case GroupFilterUngroupped:
			xml_config_file->createTextNode(groupFilterNode, "Type", "Ungroupped");
			break;
		default:
			break;
	}
}
