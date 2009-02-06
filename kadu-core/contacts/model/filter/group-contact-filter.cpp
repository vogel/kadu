 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "group-contact-filter.h"
#include "contacts/contact.h"

GroupContactFilter::GroupContactFilter() : CurrentGroup(0)
{
}

void GroupContactFilter::setGroup(Group *group)
{
	if (CurrentGroup == group)
		return;
	CurrentGroup = group;
	emit filterChanged();
}

bool GroupContactFilter::acceptContact(Contact contact)
{
	return (0 == CurrentGroup) || contact.isInGroup(CurrentGroup);
}
