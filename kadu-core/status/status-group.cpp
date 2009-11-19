/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status-group.h"

StatusGroup::StatusGroup(const QString &name, int sortIndex) :
		Name(name), SortIndex(sortIndex)
{
}

bool StatusGroup::operator < (const StatusGroup&compare) const
{
	return SortIndex < compare.SortIndex;
}

bool StatusGroup::operator == (const StatusGroup&compare) const
{
	return SortIndex == compare.SortIndex;
}

bool StatusGroup::operator != (const StatusGroup&compare) const
{
	return SortIndex != compare.SortIndex;
}
