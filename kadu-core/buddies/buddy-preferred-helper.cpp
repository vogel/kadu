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

Contact BuddyPreferredHelper::preferredContactByPendingMessages(Buddy buddy, Account account)
{
	Contact contact;
	foreach (Message message, PendingMessagesManager::instance()->pendingMessagesForBuddy(buddy))
	{
		Contact con = message.messageSender();
		if (con.ownerBuddy() == buddy)
		{
			if (account && con.contactAccount() != account)
				continue;
			if (!contact || con.currentStatus() < contact.currentStatus())
				contact = con;
		}
	}
	return contact;
}

Contact BuddyPreferredHelper::preferredContactByChatWidgets(Buddy buddy, Account account)
{
	Contact contact;
	foreach (ChatWidget *chatwidget, ChatWidgetManager::instance()->chats())
	{
		Chat chat = chatwidget->chat();
		if (chat.contacts().isEmpty())
			continue;
		Contact con = *chat.contacts().begin();
		if (con.ownerBuddy() == buddy)
		{
			if (account && con.contactAccount() != account)
				continue;
			if (!contact || con.currentStatus() < contact.currentStatus())
				contact = con;
		}
	}
	return contact;
}

Contact BuddyPreferredHelper::preferredContactByRecentChats(Buddy buddy, Account account)
{
	Contact contact;
	foreach (Chat chat, RecentChatManager::instance()->recentChats())
	{
		if (chat.contacts().isEmpty())
			continue;
		Contact con = *chat.contacts().begin();
		if (con.ownerBuddy() == buddy)
		{
			if (account && con.contactAccount() != account)
				continue;
			if (!contact || con.currentStatus() < contact.currentStatus())
				contact = con;
		}
	}
	return contact;
}

Contact BuddyPreferredHelper::preferredContactByStatus(Buddy buddy, Account account)
{
	Contact contact;
	foreach (const Contact &con, buddy.contacts())
	{
		if (account && con.contactAccount() != account)
			continue;
		if (!contact || con.currentStatus() < contact.currentStatus())
			contact = con;
	}
	return contact;
}

Contact BuddyPreferredHelper::preferredContact(Buddy buddy, Account account, bool includechats)
{
	if (buddy.isNull() || buddy.contacts().count() == 0)
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
	if (contact.isNull())
		return Account::null;
	return contact.contactAccount();
}
