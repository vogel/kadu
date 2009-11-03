/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-list.h"

BuddyList::BuddyList()
{
}

BuddyList::BuddyList(QList<Buddy> list)
{
	*this += list;
}

BuddyList::BuddyList(Buddy buddy)
{
	append(buddy);
}

bool BuddyList::operator == (const BuddyList &compare) const
{
	if (size() != compare.size())
		return false;

	foreach (Buddy buddy, compare)
		if (!contains(buddy))
			return false;
	return true;
}
