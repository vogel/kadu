/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status-type.h"

StatusType::StatusType(const QString &name, const QString &displayName, StatusGroup *statusGroup, int sortIndex) :
		Name(name), DisplayName(displayName), MyStatusGroup(statusGroup), SortIndex(sortIndex)
{
}

bool StatusType::operator < (const StatusType &compare) const
{
	if (MyStatusGroup < compare.MyStatusGroup)
		return true;
	if (MyStatusGroup == compare.MyStatusGroup)
		return SortIndex < compare.SortIndex;
	return false;
}

bool StatusType::operator == (const StatusType &compare) const
{
	return (MyStatusGroup == compare.MyStatusGroup) && (SortIndex == compare.SortIndex);
}

bool StatusType::operator != (const StatusType &compare) const
{
	return (MyStatusGroup != compare.MyStatusGroup) || (SortIndex != compare.SortIndex);
}
