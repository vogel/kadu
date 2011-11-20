/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "history-tree-item.h"

HistoryTreeItem::HistoryTreeItem() :
		Type(HistoryTypeNone)
{
}

HistoryTreeItem::HistoryTreeItem(const HistoryTreeItem &copyMe) :
		Type(copyMe.Type), ItemChat(copyMe.ItemChat), ItemBuddy(copyMe.ItemBuddy), ItemSmsRecipient(copyMe.ItemSmsRecipient)
{
}

HistoryTreeItem::HistoryTreeItem(const Chat &chat) :
		Type(HistoryTypeChat), ItemChat(chat)
{
}

HistoryTreeItem::HistoryTreeItem(const Buddy &buddy) :
		Type(HistoryTypeStatus), ItemBuddy(buddy)
{
}

HistoryTreeItem::HistoryTreeItem(const QString &smsRecipient) :
		Type(HistoryTypeSms), ItemSmsRecipient(smsRecipient)
{

}

HistoryTreeItem HistoryTreeItem::operator = (HistoryTreeItem &copyMe)
{
	Type = copyMe.Type;
	ItemChat = copyMe.ItemChat;
	ItemBuddy = copyMe.ItemBuddy;
	ItemSmsRecipient = copyMe.ItemSmsRecipient;

	return *this;
}

HistoryType HistoryTreeItem::type() const
{
	return Type;
}

Chat HistoryTreeItem::chat() const
{
	return ItemChat;
}

Buddy HistoryTreeItem::buddy() const
{
	return ItemBuddy;
}

QString HistoryTreeItem::smsRecipient() const
{
	return ItemSmsRecipient;
}
