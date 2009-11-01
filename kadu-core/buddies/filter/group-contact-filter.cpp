 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"

#include "group-contact-filter.h"

GroupContactFilter::GroupContactFilter(QObject *parent) :
		AbstractContactFilter(parent), CurrentGroup(0), AllGroupShown(true)
{
	// TODO: 0.6.6 hack, it should go thought the model itself
	connect(ContactManager::instance(), SIGNAL(contactUpdated(Contact &)),
			this, SIGNAL(filterChanged()));
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
	return (0 == CurrentGroup) // use AllGroup or UngroupedGroup
		? (AllGroupShown && contact.showInAllGroup() || !AllGroupShown && contact.groups().isEmpty())
		: contact.isInGroup(CurrentGroup);
}

void GroupContactFilter::refresh()
{
	emit filterChanged();
}

void GroupContactFilter::setAllGroupShown(bool shown)
{
	AllGroupShown = shown;
}
