/*
 * %kadu copyright begin%
 * Copyright 2010, Piotr Dąbrowski (ultr@ultr.pl)
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

#include "chat/message/pending-messages-manager.h"
#include "chat/recent-chat-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"

#include "buddy-preferred-helper.h"

namespace
{

	Contact morePreferredContactByStatus(Contact c1, Contact c2, Account account)
	{
		if (!c1 && !c2)
			return Contact::null;
		if (c1 && !c2)
		{
			if (!account || c1.contactAccount() == account)
				return c1;
			return Contact::null;
		}
		if (!c1 && c2)
		{
			if (!account || c2.contactAccount() == account)
				return c2;
			return Contact::null;
		}
		return Contact::contactWithHigherStatus(c1, c2);
	}

	Contact preferredContactByPendingMessages(Buddy buddy, Account account)
	{
		Contact result;
		foreach (Message message, PendingMessagesManager::instance()->pendingMessagesForBuddy(buddy))
		{
			Contact contact = message.messageSender();
			result = morePreferredContactByStatus(result, contact, account);
		}
		return result;
	}

	Contact preferredContactByChatWidgets(Buddy buddy, Account account)
	{
		Contact result;
		foreach (ChatWidget *chatwidget, ChatWidgetManager::instance()->chats())
		{
			Chat chat = chatwidget->chat();
			if (chat.contacts().isEmpty())
				continue;
			Contact contact = *chat.contacts().begin();
			if (contact.ownerBuddy() != buddy)
				continue;
			result = morePreferredContactByStatus(result, contact, account);
		}
		return result;
	}

	Contact preferredContactByRecentChats(Buddy buddy, Account account)
	{
		Contact result;
		foreach (Chat chat, RecentChatManager::instance()->recentChats())
		{
			if (chat.contacts().isEmpty())
				continue;
			Contact contact = *chat.contacts().begin();
			if (contact.ownerBuddy() != buddy)
				continue;
			result = morePreferredContactByStatus(result, contact, account);
		}
		return result;
	}

	Contact preferredContactByStatus(Buddy buddy, Account account)
	{
		Contact result;
		foreach (const Contact &contact, buddy.contacts())
		{
			result = morePreferredContactByStatus(result, contact, account);
		}
		return result;
	}

} // namespace

Contact BuddyPreferredHelper::preferredContact(Buddy buddy, Account account, bool includechats)
{
	if (!buddy || buddy.contacts().isEmpty())
		return Contact::null;

	Contact contact;

	if (includechats)
	{
		contact = preferredContactByPendingMessages(buddy, account);
		if (contact)
			return contact;
		contact = preferredContactByChatWidgets(buddy, account);
		if (contact)
			return contact;
		contact = preferredContactByRecentChats(buddy, account);
		if (contact)
			return contact;
	}
	contact = preferredContactByStatus(buddy, account);

	return contact;
}

Contact BuddyPreferredHelper::preferredContact(Buddy buddy, bool includechats)
{
	return BuddyPreferredHelper::preferredContact(buddy, Account::null, includechats);
}

Account BuddyPreferredHelper::preferredAccount(Buddy buddy, bool includechats)
{
	Contact contact = BuddyPreferredHelper::preferredContact(buddy, includechats);
	return contact.contactAccount();
}
