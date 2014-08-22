/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-set.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-manager.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-set.h"
#include "model/roles.h"
#include "status/status-container.h"

#include "talkable.h"

Talkable::Talkable() :
		Type(ItemNone)
{
}

Talkable::Talkable(const Buddy &buddy) :
		Type(ItemBuddy), MyBuddy(buddy)
{
}

Talkable::Talkable(const Contact &contact) :
		Type(ItemContact), MyContact(contact)
{
}

Talkable::Talkable(const Chat &chat) :
		Type(ItemChat), MyChat(chat)
{
}

Talkable::Talkable(const Talkable &copyMe)
{
	Type = copyMe.Type;

	switch (Type)
	{
		case ItemBuddy:
			MyBuddy = copyMe.MyBuddy;
			break;
		case ItemContact:
			MyContact = copyMe.MyContact;
			break;
		case ItemChat:
			MyChat = copyMe.MyChat;
			break;
		default:
			break;
	}
}

Talkable & Talkable::operator = (const Talkable &copyMe)
{
	Type = copyMe.Type;

	switch (Type)
	{
		case ItemBuddy:
			MyBuddy = copyMe.MyBuddy;
			break;
		case ItemContact:
			MyContact = copyMe.MyContact;
			break;
		case ItemChat:
			MyChat = copyMe.MyChat;
			break;
		default:
			break;
	}

	return *this;
}

bool Talkable::operator == (const Talkable &compareTo) const
{
	if (Type != compareTo.Type)
		return false;

	switch (Type)
	{
		case ItemNone: return true;
		case ItemBuddy: return MyBuddy == compareTo.MyBuddy;
		case ItemContact: return MyContact == compareTo.MyContact;
		case ItemChat: return MyChat == compareTo.MyChat;
		default:
			return false;
	}
}

bool Talkable::operator != (const Talkable &compareTo) const
{
	return !(*this == compareTo);
}

Buddy Talkable::toBuddy() const
{
	switch (Type)
	{
		case ItemBuddy: return MyBuddy;
		case ItemContact: return BuddyManager::instance()->byContact(MyContact, ActionCreateAndAdd);
		case ItemChat: return BuddyManager::instance()->byContact(toContact(), ActionCreateAndAdd);
		default:
			return Buddy::null;
	}
}

Contact Talkable::toContact() const
{
	switch (Type)
	{
		case ItemBuddy: return BuddyPreferredManager::instance()->preferredContact(MyBuddy);
		case ItemContact: return MyContact;
		case ItemChat:
			if (MyChat.contacts().size() == 1)
				return *MyChat.contacts().begin();
			else
				return Contact::null;
		default:
			return Contact::null;
	}
}

Chat Talkable::toChat() const
{
	switch (Type)
	{
		case ItemBuddy:
		{
			const Chat &chat = ChatTypeContact::findChat(BuddyPreferredManager::instance()->preferredContact2(MyBuddy), ActionCreateAndAdd);
			const Chat &buddyChat = BuddyChatManager::instance()->buddyChat(chat);
			return buddyChat ? buddyChat : chat;
		}
		case ItemContact: return ChatTypeContact::findChat(MyContact, ActionCreateAndAdd);
		case ItemChat: return MyChat;
		default:
			return Chat::null;
	}
}

bool Talkable::isEmpty() const
{
	switch (Type)
	{
		case ItemBuddy: return MyBuddy.isNull();
		case ItemContact: return MyContact.isNull();
		case ItemChat: return MyChat.isNull();
		default:
			return true;
	}
}

Avatar Talkable::avatar() const
{
	Avatar avatar;
	if (Talkable::ItemBuddy == Type)
		avatar = toBuddy().buddyAvatar();

	if (!avatar || avatar.pixmap().isNull())
		avatar = toContact().avatar(true);

	return avatar;
}

bool Talkable::isBlocked() const
{
	return toBuddy().isBlocked();
}

bool Talkable::isBlocking() const
{
	return toContact().isBlocking();
}

Account Talkable::account() const
{
	switch (Type)
	{
		case ItemChat: return MyChat.chatAccount();
		default:
			return toContact().contactAccount();
	}
}

QString Talkable::display() const
{
	switch (Type)
	{
		case ItemBuddy: return MyBuddy.display();
		case ItemContact: return MyContact.display(true);
		case ItemChat: return ChatDataExtractor::data(MyChat, Qt::DisplayRole).toString();
		default:
			return QString();
	}
}

Status Talkable::currentStatus() const
{
	if (isValidChat())
		return MyChat.chatAccount().statusContainer() ? MyChat.chatAccount().statusContainer()->status() : Status();
	else
		return toContact().currentStatus();
}

bool Talkable::isValidChat() const
{
	return ItemChat == Type && MyChat;
}

bool Talkable::isValidBuddy() const
{
	return ItemBuddy == Type && MyBuddy;
}

bool Talkable::isValidContact() const
{
	return ItemContact == Type && MyContact;
}
