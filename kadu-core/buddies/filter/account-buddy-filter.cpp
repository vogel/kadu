/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"

#include "account-buddy-filter.h"

AccountBuddyFilter::AccountBuddyFilter(Account account, QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false), CurrentAccount(account)
{
}

void AccountBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool AccountBuddyFilter::acceptBuddy(Buddy buddy)
{
	if (!Enabled)
		return true;
	return buddy.hasAccountData(CurrentAccount);
}

