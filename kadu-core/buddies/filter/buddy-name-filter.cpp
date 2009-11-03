 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"
#include "buddies/account-data/contact-account-data.h"

#include "buddy-name-filter.h"

bool BuddyNameFilter::acceptBuddy(Buddy buddy)
{
	if (Name.isEmpty())
		return true;

	if (buddy.display().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.firstName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.lastName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.nickName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.email().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (ContactAccountData *cad, buddy.accountDatas())
		if (cad->id().contains(Name, Qt::CaseInsensitive))
			return true;

	return false;
}

void BuddyNameFilter::setName(const QString &name)
{
	if (Name != name)
	{
		Name = name;
		emit filterChanged();
	}
}
