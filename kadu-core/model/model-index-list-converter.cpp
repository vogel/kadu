/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "core/core.h"
#include "message/unread-message-repository.h"
#include "model/roles.h"

#include "model-index-list-converter.h"

ModelIndexListConverter::ModelIndexListConverter(const QModelIndexList &modelIndexList) :
		ModelIndexList(modelIndexList)
{
	buildRoles();
	buildBuddies();
	buildContacts();
	buildChat();
}

RoleSet ModelIndexListConverter::roles() const
{
	return Roles;
}

BuddySet ModelIndexListConverter::buddies() const
{
	return Buddies;
}

ContactSet ModelIndexListConverter::contacts() const
{
	return Contacts;
}

Chat ModelIndexListConverter::chat() const
{
	return ComputedChat;
}

void ModelIndexListConverter::buildRoles()
{
	foreach (const QModelIndex &selection, ModelIndexList)
		Roles.insert(selection.data(ItemTypeRole).toInt());
}

void ModelIndexListConverter::buildBuddies()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
		if (buddy)
			Buddies.insert(buddy);
	}
}

void ModelIndexListConverter::buildContacts()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Contact &contact = index.data(ContactRole).value<Contact>();
		if (contact)
			Contacts.insert(contact);
	}
}

Chat ModelIndexListConverter::chatFromIndex(const QModelIndex &index) const
{
	switch (index.data(ItemTypeRole).toInt())
	{
		case ChatRole:
		{
			Chat chat = index.data(ChatRole).value<Chat>();
			if (chat.chatAccount())
				return chat;
			else
				return Chat::null;
		}
		case BuddyRole:
			return Core::instance()->unreadMessageRepository()->unreadMessageForBuddy(index.data(BuddyRole).value<Buddy>()).messageChat();
		case ContactRole:
			return Core::instance()->unreadMessageRepository()->unreadMessageForContact(index.data(ContactRole).value<Contact>()).messageChat();
	}

	return Chat::null;
}

Chat ModelIndexListConverter::chatFromBuddies() const
{
	BuddySet buddies;
	foreach (const QModelIndex &index, ModelIndexList)
		buddies.insert(index.data(BuddyRole).value<Buddy>());

	if (0 == buddies.size())
		return Chat::null;

	if (1 == buddies.size())
		return ChatTypeContact::findChat(BuddyPreferredManager::instance()->preferredContact2(*buddies.begin()), ActionCreateAndAdd);
	else
		return ChatTypeContactSet::findChat(BuddyPreferredManager::instance()->preferredContacts(buddies), ActionCreateAndAdd);
}

Chat ModelIndexListConverter::chatFromContacts(const Account &account) const
{
	if (!account)
		return Chat::null;

	ContactSet contacts;
	foreach (const QModelIndex &index, ModelIndexList)
	{
		Contact contact = contactForAccount(index, account);
		if (!contact)
			return Chat::null;

		contacts.insert(contact);
	}

	if (contacts.isEmpty())
		return Chat::null;
	return 1 == contacts.size()
			? ChatTypeContact::findChat(*contacts.constBegin(), ActionCreateAndAdd)
			: ChatTypeContactSet::findChat(contacts, ActionCreateAndAdd);
}

Account ModelIndexListConverter::commonAccount() const
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		if (index.data(ItemTypeRole) == ContactRole)
		{
			const Contact &contact = index.data(ContactRole).value<Contact>();
			if (contact)
				return contact.contactAccount();
		}
	}

	return Account::null;
}

Contact ModelIndexListConverter::contactForAccount(const QModelIndex &index, const Account &account) const
{
	if (index.data(ItemTypeRole) == BuddyRole)
		return BuddyPreferredManager::instance()->preferredContact(index.data(BuddyRole).value<Buddy>(), account);

	const Contact &contact = index.data(ContactRole).value<Contact>();
	if (contact.contactAccount() == account)
		return contact;

	return Contact::null;
}

void ModelIndexListConverter::buildChat()
{
	if (ModelIndexList.size() == 1)
	{
		ComputedChat = chatFromIndex(ModelIndexList.at(0));
		if (ComputedChat)
			return;
	}

	// more than one item selected and a Chat selected
	// we can not return one chat in this situation
	if (Roles.contains(ChatRole))
		return;

	// only buddies selected
	if (Roles.contains(BuddyRole) && Roles.size() == 1)
	{
		ComputedChat = chatFromBuddies();
		return;
	}

	// buddies and contacts selected
	if (Roles.contains(ContactRole))
		ComputedChat = chatFromContacts(commonAccount());
}
