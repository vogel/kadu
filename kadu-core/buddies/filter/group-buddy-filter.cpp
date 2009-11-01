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

#include "group-buddy-filter.h"

GroupBuddyFilter::GroupBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent), CurrentGroup(0), AllGroupShown(true)
{
	// TODO: 0.6.6 hack, it should go thought the model itself
	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SIGNAL(filterChanged()));
}

void GroupBuddyFilter::setGroup(Group *group)
{
	if (CurrentGroup == group)
		return;

	CurrentGroup = group;
	emit filterChanged();
}

bool GroupBuddyFilter::acceptBuddy(Buddy contact)
{
	return (0 == CurrentGroup) // use AllGroup or UngroupedGroup
		? (AllGroupShown && contact.showInAllGroup() || !AllGroupShown && contact.groups().isEmpty())
		: contact.isInGroup(CurrentGroup);
}

void GroupBuddyFilter::refresh()
{
	emit filterChanged();
}

void GroupBuddyFilter::setAllGroupShown(bool shown)
{
	AllGroupShown = shown;
}
