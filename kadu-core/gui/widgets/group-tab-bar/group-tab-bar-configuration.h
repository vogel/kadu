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

#ifndef GROUP_TAB_BAR_CONFIGURATION_H
#define GROUP_TAB_BAR_CONFIGURATION_H

#include <QtCore/QVector>

#include "talkable/filter/group-filter.h"

class GroupTabBarConfiguration
{
	bool DisplayGroupTabs;
	bool ShowGroupTabEverybody;
	bool AlwaysShowGroupTabUngroupped;
	int CurrentGroupTab;
	QVector<GroupFilter> GroupFilters;

public:
	GroupTabBarConfiguration();
	GroupTabBarConfiguration(const GroupTabBarConfiguration &) = default;

	GroupTabBarConfiguration & operator = (const GroupTabBarConfiguration &) = default;

	void setDisplayGroupTabs(bool displayGroupTabs);
	bool displayGroupTabs() const;

	void setShowGroupTabEverybody(int showGroupTabEverybody);
	int showGroupTabEverybody() const;

	void setAlwaysShowGroupTabUngroupped(int alwaysShowGroupTabUngroupped);
	bool alwaysShowGroupTabUngroupped() const;

	void setCurrentGroupTab(int currentGroupTab);
	int currentGroupTab() const;

	void setGroupFilters(QVector<GroupFilter> groupFilters);
	QVector<GroupFilter> groupFilters() const;

};

#endif // GROUP_TAB_BAR_CONFIGURATION_H
