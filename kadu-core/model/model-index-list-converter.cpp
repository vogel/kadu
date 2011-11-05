/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "model/roles.h"

#include "model-index-list-converter.h"

ModelIndexListConverter::ModelIndexListConverter(const QModelIndexList &modelIndexList) :
		ModelIndexList(modelIndexList)
{
	buildBuddySet();
	buildContactSet();
	buildChat();
}

BuddySet ModelIndexListConverter::buddySet() const
{
	return ComputedBuddySet;
}

ContactSet ModelIndexListConverter::contactSet() const
{
	return ComputedContactSet;
}

Chat ModelIndexListConverter::chat() const
{
	return ComputedChat;
}

void ModelIndexListConverter::buildBuddySet()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
		if (buddy)
			ComputedBuddySet.insert(buddy);
	}
}

void ModelIndexListConverter::buildContactSet()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Contact &contact = index.data(ContactRole).value<Contact>();
		if (contact)
			ComputedContactSet.insert(contact);
	}
}

Chat ModelIndexListConverter::chatByPendingMessages(const QModelIndex &index) const
{
	if (index.data(ItemTypeRole) == BuddyRole)
		return PendingMessagesManager::instance()->chatForBuddy(index.data(BuddyRole).value<Buddy>());
	else
		return PendingMessagesManager::instance()->chatForContact(index.data(ContactRole).value<Contact>());
}

Account ModelIndexListConverter::commonAccount() const
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		if (index.data(ItemTypeRole) != BuddyRole)
		{
			const Contact &contact = index.data(ContactRole).value<Contact>();
			if (contact)
				return contact.contactAccount();
		}
	}

	return Account::null;
}

// TODO 0.11.0: This method is too big. Review and split
void ModelIndexListConverter::buildChat()
{
	if (ModelIndexList.size() == 1)
	{
		ComputedChat = chatByPendingMessages(ModelIndexList.at(0));
		if (ComputedChat)
			return;
	}

	const Account &account = commonAccount();
	if (!account)
	{
		BuddySet buddies;
		foreach (const QModelIndex &index, ModelIndexList)
			if (index.data(ItemTypeRole) == BuddyRole)
				buddies.insert(index.data(BuddyRole).value<Buddy>());
			else
				return;

		ComputedChat = ChatManager::instance()->findChat(buddies, true);
		return;
	}

	ContactSet contacts;
	foreach (const QModelIndex &index, ModelIndexList)
	{
		if (index.data(ItemTypeRole) == BuddyRole)
		{
			const Contact &contact = BuddyPreferredManager::instance()->preferredContact(index.data(BuddyRole).value<Buddy>(), account);
			if (!contact)
				return;

			contacts.insert(contact);
		}
		else
		{
			const Contact &contact = index.data(ContactRole).value<Contact>();
			if (!contact)
				return;

			if (contact.contactAccount() == account)
				contacts.insert(contact);
			else
				return;
		}
	}

	ComputedChat = ChatManager::instance()->findChat(contacts, true);
}
