/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-mergable-filter.h"

BuddyMergableFilter::BuddyMergableFilter(Buddy contact, QObject *parent) :
		AbstractBuddyFilter(parent), MyContact(contact)
{
	Accounts = MyContact.accounts().toSet();
}

BuddyMergableFilter::~BuddyMergableFilter()
{
}

void BuddyMergableFilter::setContact(Buddy contact)
{
	if (MyContact == contact)
		return;

	MyContact = contact;
	Accounts = contact.accounts().toSet();

	emit filterChanged();
}

bool BuddyMergableFilter::acceptBuddy(Buddy contact)
{
	return contact.accounts().toSet().intersect(Accounts).empty();
}
