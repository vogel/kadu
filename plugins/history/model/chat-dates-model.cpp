/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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


#include "chat/chat.h"
#include "model/roles.h"

#include "dates-model-item.h"
#include "history-tree-item.h"
#include "history.h"

#include "chat-dates-model.h"

ChatDatesModel::ChatDatesModel(const Chat &chat, const QList<DatesModelItem> &dates, QObject *parent) :
		QAbstractListModel(parent), MyChat(chat), Dates(dates)
{
}

ChatDatesModel::~ChatDatesModel()
{
}

int ChatDatesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 4;
}

int ChatDatesModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Dates.size();
}

QVariant ChatDatesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation != Qt::Horizontal)
		return QVariant();

	switch (section)
	{
		case 0: return tr("Chat");
		case 1: return tr("Title");
		case 2: return tr("Date");
		case 3: return tr("Length");
	}

	return QVariant();
}

QVariant ChatDatesModel::data(const QModelIndex &index, int role) const
{
	if (!MyChat)
		return QVariant();

	int col = index.column();
	int row = index.row();

	if (row < 0 || row >= Dates.size())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (col)
			{
				case 0: return MyChat.name();
				case 1: return Dates.at(row).Title;
				case 2: return Dates.at(row).Date.toString("dd.MM.yyyy");
				case 3: return Dates.at(row).Count;
			}

			return QVariant();
		}

		case HistoryItemRole: return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(MyChat));
		case ChatRole: return QVariant::fromValue<Chat>(MyChat);
		case DateRole: return Dates.at(row).Date;
	}

	return QVariant();
}

void ChatDatesModel::setChat(const Chat &chat)
{
	MyChat = chat;
}

void ChatDatesModel::setDates(const QList<DatesModelItem> &dates)
{
	if (!Dates.isEmpty())
	{
		beginRemoveRows(QModelIndex(), 0, Dates.size() - 1);
		Dates.clear();
		endRemoveRows();
	}

	if (!dates.isEmpty())
	{
		beginInsertRows(QModelIndex(), 0, dates.size() - 1);
		Dates = dates;
		endInsertRows();
	}
}

QModelIndex ChatDatesModel::indexForDate(const QDate &date)
{
	int i = 0;
	foreach (const DatesModelItem &item, Dates)
	{
		if (item.Date == date)
			return index(i);
		++i;
	}

	return index(-1);
}
