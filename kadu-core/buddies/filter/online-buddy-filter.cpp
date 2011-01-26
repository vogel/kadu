/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "accounts/account.h"
#include "buddies/buddy-preferred-manager.h"
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

bool OnlineBuddyFilter::acceptBuddy(const Buddy &buddy)
{
	if (!Enabled)
		return true;

	Contact preferredContact = BuddyPreferredManager::instance()->preferredContact(buddy, false);
	if (preferredContact.isNull())
		return false;

	Status status = preferredContact.currentStatus();
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

