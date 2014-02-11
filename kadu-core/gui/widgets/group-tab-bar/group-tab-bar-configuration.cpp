/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "group-tab-bar-configuration.h"

GroupTabBarConfiguration::GroupTabBarConfiguration() :
		DisplayGroupTabs{true}, ShowGroupTabEverybody{true}, AlwaysShowGroupTabUngroupped{false}, CurrentGroupTab{0}
{
}
void GroupTabBarConfiguration::setDisplayGroupTabs(bool displayGroupTabs)
{
	DisplayGroupTabs = displayGroupTabs;
}

bool GroupTabBarConfiguration::displayGroupTabs() const
{
	return DisplayGroupTabs;
}

void GroupTabBarConfiguration::setShowGroupTabEverybody(int showGroupTabEverybody)
{
	ShowGroupTabEverybody = showGroupTabEverybody;
}

int GroupTabBarConfiguration::showGroupTabEverybody() const
{
	return ShowGroupTabEverybody;
}

void GroupTabBarConfiguration::setAlwaysShowGroupTabUngroupped(int alwaysShowGroupTabUngroupped)
{
	AlwaysShowGroupTabUngroupped = alwaysShowGroupTabUngroupped;
}

bool GroupTabBarConfiguration::alwaysShowGroupTabUngroupped() const
{
	return AlwaysShowGroupTabUngroupped;
}

void GroupTabBarConfiguration::setCurrentGroupTab(int currentGroupTab)
{
	CurrentGroupTab = currentGroupTab;
}

int GroupTabBarConfiguration::currentGroupTab() const
{
	return CurrentGroupTab;
}

void GroupTabBarConfiguration::setGroupFilters(QVector<GroupFilter> groupFilters)
{
	GroupFilters = groupFilters;
}

QVector<GroupFilter> GroupTabBarConfiguration::groupFilters() const
{
	return GroupFilters;
}
