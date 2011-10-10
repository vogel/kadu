/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status-type.h"

bool StatusType::lessThan(const StatusType *left, const StatusType *right)
{
	return *left < *right;
}

StatusType::StatusType(const QString &name, const QString &displayName, const QString &iconName, StatusTypeGroup typeGroup, int sortIndex) :
		Name(name), DisplayName(displayName), IconName(iconName), TypeGroup(typeGroup), SortIndex(sortIndex)
{
}

bool StatusType::operator < (const StatusType &compare) const
{
	if (TypeGroup < compare.TypeGroup)
		return true;
	if (TypeGroup == compare.TypeGroup)
		return SortIndex < compare.SortIndex;
	return false;
}

bool StatusType::operator == (const StatusType &compare) const
{
	return (TypeGroup == compare.TypeGroup) && (SortIndex == compare.SortIndex);
}

bool StatusType::operator != (const StatusType &compare) const
{
	return (TypeGroup != compare.TypeGroup) || (SortIndex != compare.SortIndex);
}
