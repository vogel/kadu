/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "talkable/filter/group-filter-type.h"

#include "group-filter.h"

GroupFilter::GroupFilter() :
		FilterType(GroupFilterInvalid)
{
}

GroupFilter::GroupFilter(GroupFilterType filterType) :
		FilterType(filterType)
{
}

GroupFilter::GroupFilter(const Group &group) :
		FilterType(GroupFilterRegular), MyGroup(group)
{
}

bool GroupFilter::operator == (const GroupFilter &groupFilter) const
{
	if (groupFilter.filterType() != FilterType)
		return false;
	return groupFilter.group() == MyGroup;
}

bool GroupFilter::isValid() const
{
	return GroupFilterInvalid != FilterType;
}

GroupFilterType GroupFilter::filterType() const
{
	return FilterType;
}

Group GroupFilter::group() const
{
	return MyGroup;
}
