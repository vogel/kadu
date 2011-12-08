/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "buddies/buddy-preferred-manager.h"
#include "contacts/contact-set.h"
#include "model/roles.h"

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
		case ItemContact: return MyContact.ownerBuddy();
		case ItemChat: return toContact().ownerBuddy();
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

Status Talkable::currentStatus() const
{
	return toContact().currentStatus();
}
