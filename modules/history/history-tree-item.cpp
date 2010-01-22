/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"
#include "chat/chat.h"

#include "history-tree-item.h"

HistoryTreeItem::HistoryTreeItem() :
		Type(HistoryTypeNone)
{
}

HistoryTreeItem::HistoryTreeItem(const HistoryTreeItem &copyMe) :
		Type(copyMe.Type), ItemChat(copyMe.ItemChat), ItemBuddy(copyMe.ItemBuddy)
{
}

HistoryTreeItem::HistoryTreeItem(Chat chat) :
		Type(HistoryTypeChat), ItemChat(chat)
{
}

HistoryTreeItem::HistoryTreeItem(Buddy buddy) :
		Type(HistoryTypeStatus), ItemBuddy(buddy)
{
}

HistoryTreeItem HistoryTreeItem::operator = (HistoryTreeItem &copyMe)
{
	Type = copyMe.Type;
	ItemChat = copyMe.ItemChat;
	ItemBuddy = copyMe.ItemBuddy;

	return *this;
}

HistoryType HistoryTreeItem::type()
{
	return Type;
}

Chat HistoryTreeItem::chat()
{
	return ItemChat;
}

Buddy HistoryTreeItem::buddy()
{
	return ItemBuddy;
}
