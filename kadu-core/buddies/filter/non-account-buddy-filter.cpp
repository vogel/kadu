/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"

#include "non-account-buddy-filter.h"

NonAccountBuddyFilter::NonAccountBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent), CurrentAccount(0)
{
}

NonAccountBuddyFilter::~NonAccountBuddyFilter()
{
}

void NonAccountBuddyFilter::setAccount(Account account)
{
	if (CurrentAccount != account)
	{
		CurrentAccount = account;
		emit filterChanged();
	}
}

bool NonAccountBuddyFilter::acceptBuddy(Buddy contact)
{
	return CurrentAccount.isNull() || !contact.hasAccountData(CurrentAccount);
}
