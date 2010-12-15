/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"

#include "open-chat-with-contact-list-runner.h"

BuddyList OpenChatWithContactListRunner::matchingContacts(const QString &query)
{
	BuddyList matchedContacts;

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
	{
		bool found = false;
		foreach (const Contact &data, buddy.contacts())
			if (data.id().contains(query, Qt::CaseInsensitive))
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
