/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HISTORY_TREE_ITEM_H
#define HISTORY_TREE_ITEM_H

#include <QtCore/QVariant>

#include "model/history-type.h"

class Buddy;
class Chat;

class HistoryTreeItem
{
	HistoryType Type;
	Chat ItemChat;
	Buddy ItemBuddy;
	QString ItemSmsRecipient;

public:
	HistoryTreeItem();
	HistoryTreeItem(const HistoryTreeItem &copyMe);

	explicit HistoryTreeItem(const Chat &chat);
	explicit HistoryTreeItem(const Buddy &buddy);
	explicit HistoryTreeItem(const QString &smsRecipient);

	HistoryTreeItem operator = (HistoryTreeItem &copyMe);

	HistoryType type() const;
	Chat chat() const;
	Buddy buddy() const;
	QString smsRecipient() const;

};

Q_DECLARE_METATYPE(HistoryTreeItem)

#endif // HISTORY_TREE_ITEM_H
