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

#include "online-buddy-filter.h"

OnlineBuddyFilter::OnlineBuddyFilter(QObject *parent)
	: AbstractBuddyFilter(parent), Enabled(false)
{
}

void OnlineBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool OnlineBuddyFilter::acceptBuddy(Buddy buddy)
{
	if (!Enabled)
		return true;

	Contact prefferedContact = buddy.prefferedContact();
	if (prefferedContact.isNull())
		return false;

	Status status = prefferedContact.currentStatus();
	return !status.isDisconnected();
}


/*
	if (config_file.readBoolEntry("General", "ShowBlocking") != showBlocking)
		changeDisplayingBlocking(!showBlocking);
	if (config_file.readBoolEntry("General", "ShowBlocked") != showBlocked)
		changeDisplayingBlocked(!showBlocked);*/
// 	if (config_file.readBoolEntry("General", "ShowOffline") != showOffline)
// 		changeDisplayingOffline(kadu->userbox(), !showOffline);
//  	if (config_file.readBoolEntry("General", "ShowWithoutDescription") != showWithoutDescription)
//  		changeDisplayingWithoutDescription(kadu->userbox(), !showWithoutDescription);

// BlockedUsers::BlockedUsers() : UserGroup()
// 		if (user.usesProtocol("Gadu") && user.protocolData("Gadu", "Blocking").toBool())
// 			addUser(user);

