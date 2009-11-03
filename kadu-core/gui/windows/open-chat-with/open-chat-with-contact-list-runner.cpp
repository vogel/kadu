/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/account-data/contact-account-data.h"

#include "open-chat-with-contact-list-runner.h"

BuddyList OpenChatWithContactListRunner::matchingContacts(const QString &query)
{
	BuddyList matchedContacts;

	foreach (Buddy buddy, BuddyManager::instance()->buddies())
	{
		bool found = false;
		foreach (ContactAccountData *data, buddy.accountDatas())
			if (data->id().contains(query, Qt::CaseInsensitive))
			{
				matchedContacts.append(buddy);
				found = true;
				break;
			}

		if (found)
			continue;
		
		if (buddy.firstName().contains(query, Qt::CaseInsensitive) ||
				buddy.lastName().contains(query, Qt::CaseInsensitive) ||
				buddy.display().contains(query, Qt::CaseInsensitive) ||
				buddy.nickName().contains(query, Qt::CaseInsensitive))
			matchedContacts.append(buddy);
	}

	return matchedContacts;
}
