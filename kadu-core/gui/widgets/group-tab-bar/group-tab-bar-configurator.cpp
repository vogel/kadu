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

#include <QtWidgets/QApplication>

#include "buddies/group-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/configuration-api.h"
#include "gui/widgets/group-tab-bar/group-tab-bar.h"
#include "gui/widgets/group-tab-bar/group-tab-bar-configuration.h"
#include "kadu-application.h"

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
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Look", "ShowGroupAll", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Look", "ShowGroupTabUngroupped", false);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Look", "DisplayGroupTabs", true);
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

	configuration.setDisplayGroupTabs(KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "DisplayGroupTabs", true));
	configuration.setShowGroupTabEverybody(KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "ShowGroupAll", true));
	configuration.setAlwaysShowGroupTabUngroupped(KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("Look", "AlwaysShowGroupTabUngroupped", true));
	configuration.setCurrentGroupTab(KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("Look", "CurrentGroupTab", 0));
	configuration.setGroupFilters(loadGroupFilters(configuration.showGroupTabEverybody()));

	return configuration;
}

void GroupTabBarConfigurator::storeConfiguration()
{
	if (!ConfigurableGroupTabBar)
		return;

	auto configuration = ConfigurableGroupTabBar.data()->configuration();
	KaduApplication::instance()->deprecatedConfigurationApi()->writeEntry("Look", "CurrentGroupTab", configuration.currentGroupTab());

	storeGroupFilters(configuration.groupFilters());

	KaduApplication::instance()->configurationApi()->sync(); // TODO: fix whole configuration system
}

QVector<GroupFilter> GroupTabBarConfigurator::loadGroupFilters(bool showGroupTabEverybody) const
{
	auto groupTabBarNode = KaduApplication::instance()->configurationApi()->getNode("GroupTabBar", ConfigurationApi::ModeGet);
	if (groupTabBarNode.isNull())
		return import_0_12_groupFilters(showGroupTabEverybody);

	auto result = QVector<GroupFilter>();
	auto groupFilterNodes = KaduApplication::instance()->configurationApi()->getNodes(groupTabBarNode, "GroupFilter");
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
	auto type = KaduApplication::instance()->configurationApi()->getTextNode(element, "Type");
	auto groupUuid = KaduApplication::instance()->configurationApi()->getTextNode(element, "Group");

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
			? KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("Look", "AllGroupTabPosition", 0)
			: KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("Look", "UngroupedGroupTabPosition", 0);

	auto groups = GroupManager::instance()->items().toList();
	qStableSort(groups.begin(), groups.end(), [](const Group &a, const Group &b){ return a.tabPosition() < b.tabPosition(); });

	foreach (const auto &group, groups)
		result.append(GroupFilter(group));

	result.insert(qBound(0, position, result.size()), GroupFilter(showGroupTabEverybody ? GroupFilterEverybody : GroupFilterUngroupped));

	return result;
}

void GroupTabBarConfigurator::storeGroupFilters(const QVector<GroupFilter> &groupFilters)
{
	auto groupTabBarNode = KaduApplication::instance()->configurationApi()->getNode("GroupTabBar", ConfigurationApi::ModeCreate);
	foreach (const auto &groupFilter, groupFilters)
		storeGroupFilter(groupTabBarNode, groupFilter);
}

void GroupTabBarConfigurator::storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter)
{
	if (GroupFilterInvalid == groupFilter.filterType())
		return;

	auto groupFilterNode = KaduApplication::instance()->configurationApi()->getNode(parentElement, "GroupFilter", ConfigurationApi::ModeAppend);
	switch (groupFilter.filterType())
	{
		case GroupFilterRegular:
			KaduApplication::instance()->configurationApi()->createTextNode(groupFilterNode, "Type", "Regular");
			KaduApplication::instance()->configurationApi()->createTextNode(groupFilterNode, "Group", groupFilter.group().uuid().toString());
			break;
		case GroupFilterEverybody:
			KaduApplication::instance()->configurationApi()->createTextNode(groupFilterNode, "Type", "Everybody");
			break;
		case GroupFilterUngroupped:
			KaduApplication::instance()->configurationApi()->createTextNode(groupFilterNode, "Type", "Ungroupped");
			break;
		default:
			break;
	}
}
