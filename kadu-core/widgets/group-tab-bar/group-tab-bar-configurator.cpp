/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "group-tab-bar-configurator.h"

#include "buddies/group-manager.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "widgets/group-tab-bar/group-tab-bar-configuration.h"
#include "widgets/group-tab-bar/group-tab-bar.h"

#include <QtWidgets/QApplication>

GroupTabBarConfigurator::GroupTabBarConfigurator(QObject *parent) :
		QObject{parent}
{
}

GroupTabBarConfigurator::~GroupTabBarConfigurator()
{
}

void GroupTabBarConfigurator::setApplication(Application *application)
{
	m_application = application;
}

void GroupTabBarConfigurator::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void GroupTabBarConfigurator::setGroupManager(GroupManager *groupManager)
{
	m_groupManager = groupManager;
}

void GroupTabBarConfigurator::init()
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
	m_configuration->deprecatedApi()->addVariable("Look", "DisplayGroupTabs", true);
	m_configuration->deprecatedApi()->addVariable("Look", "ShowGroupAll", true);
	m_configuration->deprecatedApi()->addVariable("Look", "AlwaysShowGroupTabUngroupped", false);
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

	configuration.setDisplayGroupTabs(m_configuration->deprecatedApi()->readBoolEntry("Look", "DisplayGroupTabs", true));
	configuration.setShowGroupTabEverybody(m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowGroupAll", true));
	configuration.setAlwaysShowGroupTabUngroupped(m_configuration->deprecatedApi()->readBoolEntry("Look", "AlwaysShowGroupTabUngroupped", true));
	configuration.setCurrentGroupTab(m_configuration->deprecatedApi()->readNumEntry("Look", "CurrentGroupTab", 0));
	configuration.setGroupFilters(loadGroupFilters(configuration.showGroupTabEverybody()));

	return configuration;
}

void GroupTabBarConfigurator::storeConfiguration()
{
	if (!ConfigurableGroupTabBar)
		return;

	auto configuration = ConfigurableGroupTabBar.data()->configuration();
	m_configuration->deprecatedApi()->writeEntry("Look", "CurrentGroupTab", configuration.currentGroupTab());

	storeGroupFilters(configuration.groupFilters());

	m_application->flushConfiguration(); // TODO: fix whole configuration system
}

QVector<GroupFilter> GroupTabBarConfigurator::loadGroupFilters(bool showGroupTabEverybody) const
{
	auto groupTabBarNode = m_configuration->api()->getNode("GroupTabBar", ConfigurationApi::ModeGet);
	if (groupTabBarNode.isNull())
		return import_0_12_groupFilters(showGroupTabEverybody);

	auto result = QVector<GroupFilter>();
	auto groupFilterNodes = m_configuration->api()->getNodes(groupTabBarNode, "GroupFilter");
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
	auto type = m_configuration->api()->getTextNode(element, "Type");
	auto groupUuid = m_configuration->api()->getTextNode(element, "Group");

	if (type == "Regular")
		return GroupFilter(m_groupManager->byUuid(groupUuid));
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
			? m_configuration->deprecatedApi()->readNumEntry("Look", "AllGroupTabPosition", 0)
			: m_configuration->deprecatedApi()->readNumEntry("Look", "UngroupedGroupTabPosition", 0);

	auto groups = m_groupManager->items().toList();
	qStableSort(groups.begin(), groups.end(), [](const Group &a, const Group &b){ return a.tabPosition() < b.tabPosition(); });

	foreach (const auto &group, groups)
		result.append(GroupFilter(group));

	result.insert(qBound(0, position, result.size()), GroupFilter(showGroupTabEverybody ? GroupFilterEverybody : GroupFilterUngroupped));

	return result;
}

void GroupTabBarConfigurator::storeGroupFilters(const QVector<GroupFilter> &groupFilters)
{
	auto groupTabBarNode = m_configuration->api()->getNode("GroupTabBar", ConfigurationApi::ModeCreate);
	foreach (const auto &groupFilter, groupFilters)
		storeGroupFilter(groupTabBarNode, groupFilter);
}

void GroupTabBarConfigurator::storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter)
{
	if (GroupFilterInvalid == groupFilter.filterType())
		return;

	auto groupFilterNode = m_configuration->api()->getNode(parentElement, "GroupFilter", ConfigurationApi::ModeAppend);
	switch (groupFilter.filterType())
	{
		case GroupFilterRegular:
			m_configuration->api()->createTextNode(groupFilterNode, "Type", "Regular");
			m_configuration->api()->createTextNode(groupFilterNode, "Group", groupFilter.group().uuid().toString());
			break;
		case GroupFilterEverybody:
			m_configuration->api()->createTextNode(groupFilterNode, "Type", "Everybody");
			break;
		case GroupFilterUngroupped:
			m_configuration->api()->createTextNode(groupFilterNode, "Type", "Ungroupped");
			break;
		default:
			break;
	}
}
