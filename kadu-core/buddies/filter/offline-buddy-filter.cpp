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

#include "offline-buddy-filter.h"

OfflineBuddyFilter::OfflineBuddyFilter(QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false)
{
}

void OfflineBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool OfflineBuddyFilter::acceptBuddy(Buddy buddy)
{
	if (!Enabled)
		return true;

	Account prefferedAccount = buddy.prefferedAccount();
	if (prefferedAccount.isNull())
		return false;

	Status status = buddy.accountData(prefferedAccount)->status();
	return !status.isDisconnected();
}
