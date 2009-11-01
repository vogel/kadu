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

#include "contact-name-filter.h"

bool ContactNameFilter::acceptContact(Contact contact)
{
	if (Name.isEmpty())
		return true;

	if (contact.display().contains(Name, Qt::CaseInsensitive))
		return true;
	if (contact.firstName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (contact.lastName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (contact.nickName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (contact.email().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (ContactAccountData *cad, contact.accountDatas())
		if (cad->id().contains(Name, Qt::CaseInsensitive))
			return true;

	return false;
}

void ContactNameFilter::setName(const QString &name)
{
	if (Name != name)
	{
		Name = name;
		emit filterChanged();
	}
}
