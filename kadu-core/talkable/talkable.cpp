/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "buddies/buddy-set.h"
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

Buddy Talkable::buddy() const
{
	return MyBuddy;
}

Chat Talkable::chat() const
{
	return MyChat;
}

Contact Talkable::contact() const
{
	return MyContact;
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
