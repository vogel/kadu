/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "buddies/account-data/contact-account-data.h"

#include "anonymous-buddy-filter.h"

AnonymousBuddyFilter::AnonymousBuddyFilter(QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false)
{
}

void AnonymousBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool AnonymousBuddyFilter::acceptBuddy(Buddy contact)
{
	if (!Enabled)
		return true;

	Account prefferedAccount = contact.prefferedAccount();
	if (prefferedAccount.isNull())
		return false;

	return !contact.isAnonymous();
}
