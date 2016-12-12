/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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


#include "open-chat-with-contact-list-runner.h"

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"

OpenChatWithContactListRunner::OpenChatWithContactListRunner(QObject *parent) :
		QObject{parent}
{
}

OpenChatWithContactListRunner::~OpenChatWithContactListRunner()
{
}

void OpenChatWithContactListRunner::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

BuddyList OpenChatWithContactListRunner::matchingContacts(const QString &query)
{
	BuddyList matchedContacts;

	for (auto const &buddy : m_buddyManager->items())
	{
		auto found = false;
		for (auto const &data : buddy.contacts())
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
