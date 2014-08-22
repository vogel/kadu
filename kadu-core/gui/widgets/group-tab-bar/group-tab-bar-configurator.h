/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GROUP_TAB_BAR_CONFIGURATOR_H
#define GROUP_TAB_BAR_CONFIGURATOR_H

#include <QtCore/QPointer>

#include "configuration/configuration-aware-object.h"

class QDomElement;

class GroupFilter;
class GroupTabBar;
class GroupTabBarConfiguration;

class GroupTabBarConfigurator : private ConfigurationAwareObject
{
	QPointer<GroupTabBar> ConfigurableGroupTabBar;

	void createDefaultConfiguration();

	QVector<GroupFilter> loadGroupFilters(bool showGroupTabEverybody) const;
	GroupFilter loadGroupFilter(QDomElement element) const;
	QVector<GroupFilter> import_0_12_groupFilters(bool showGroupTabEverybody) const;

	void storeGroupFilters(const QVector<GroupFilter> &groupFilters);
	void storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter);

protected:
	virtual void configurationUpdated();

public:
	GroupTabBarConfigurator();

	void setGroupTabBar(GroupTabBar *groupTabBar);

	GroupTabBarConfiguration loadConfiguration() const;
	void storeConfiguration();

};

#endif // GROUP_TAB_BAR_CONFIGURATOR_H
