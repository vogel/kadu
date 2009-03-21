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

GroupContactFilter::GroupContactFilter(QObject *parent)
	: AbstractContactFilter(parent), CurrentGroup(0), AllGroupShown(true)
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
	return (0 == CurrentGroup && (AllGroupShown && contact.showInAllGroup() || !AllGroupShown && contact.groups().isEmpty())) || contact.isInGroup(CurrentGroup);
}

void GroupContactFilter::refresh()
{
	emit filterChanged();
}

void GroupContactFilter::setAllGroupShown(bool shown)
{
	AllGroupShown = shown;
}
