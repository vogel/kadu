/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "contacts/contact.h"

#include "online-and-description-buddy-filter.h"

OnlineAndDescriptionBuddyFilter::OnlineAndDescriptionBuddyFilter(QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false)
{
}

void OnlineAndDescriptionBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool OnlineAndDescriptionBuddyFilter::acceptBuddy(Buddy buddy)
{
	if (!Enabled)
		return true;

	Contact prefferedContact = buddy.prefferedContact();
	if (prefferedContact.isNull())
		return false;

	Status status = prefferedContact.currentStatus();
	return !status.isDisconnected() || !status.description().isEmpty();
}
