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

#include <QtCore/QString>

class GroupTabBarConfiguration
{
	bool DisplayGroupTabs;
	bool ShowGroupTabEverybody;
	bool ShowGroupTabUngroupped;
	int CurrentGroupTab;

public:
	GroupTabBarConfiguration();
	GroupTabBarConfiguration(const GroupTabBarConfiguration &copyMe) = default;

	GroupTabBarConfiguration & operator = (const GroupTabBarConfiguration &copyMe) = default;

	void setDisplayGroupTabs(bool displayGroupTabs);
	bool displayGroupTabs() const;

	void setShowGroupTabEverybody(int showGroupTabEverybody);
	int showGroupTabEverybody() const;

	void setShowGroupTabUngroupped(int showGroupTabUngroupped);
	bool showGroupTabUngroupped() const;

	void setCurrentGroupTab(int currentGroupTab);
	int currentGroupTab() const;

};

#endif // GROUP_TAB_BAR_CONFIGURATION_H