/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-mergable-filter.h"

BuddyMergableFilter::BuddyMergableFilter(Buddy buddy, QObject *parent) :
		AbstractBuddyFilter(parent), MyBuddy(buddy)
{
	Accounts = MyBuddy.accounts().toSet();
}

BuddyMergableFilter::~BuddyMergableFilter()
{
}

void BuddyMergableFilter::setContact(Buddy buddy)
{
	if (MyBuddy == buddy)
		return;

	MyBuddy = buddy;
	Accounts = buddy.accounts().toSet();

	emit filterChanged();
}

bool BuddyMergableFilter::acceptBuddy(Buddy buddy)
{
	return buddy.accounts().toSet().intersect(Accounts).empty();
}
