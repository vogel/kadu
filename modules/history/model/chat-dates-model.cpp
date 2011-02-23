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

#include <QtGui/QTextDocument>

#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "chat/message/message.h"
#include "model/roles.h"

#include "history-tree-item.h"
#include "history.h"

#include "chat-dates-model.h"

ChatDatesModel::ChatDatesModel(const Chat &chat, const QList<QDate> &dates, QObject *parent) :
		QAbstractListModel(parent), MyChat(chat), Dates(dates)
{
	Cache = new QMap<QDate, ItemCachedData>();
}

ChatDatesModel::~ChatDatesModel()
{
	delete Cache;
	Cache = 0;
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

QString ChatDatesModel::fetchTitle(const QDate &date) const
{
	QList<Message> messages = History::instance()->messages(MyChat, date, 1);
	if (messages.size() == 0)
		return QString();

	Message firstMessage = messages.at(0);
	QTextDocument document;
	document.setHtml(firstMessage.content());
	FormattedMessage formatted = FormattedMessage::parse(&document);
	QString title = formatted.toPlain();

	if (title.length() > 20)
	{
		title.truncate(20);
		title += " ...";
	}

	return title;
}

int ChatDatesModel::fetchSize(const QDate &date) const
{
	return History::instance()->messagesCount(MyChat, date);
}

ChatDatesModel::ItemCachedData ChatDatesModel::fetchCachedData(const QDate &date) const
{
	if (Cache->contains(date))
		return Cache->value(date);

	ItemCachedData cache;
	cache.title = fetchTitle(date);
	cache.size = fetchSize(date);
	Cache->insert(date, cache);

	return cache;
}

QVariant ChatDatesModel::data(const QModelIndex &index, int role) const
{
	if (!MyChat)
		return QVariant();

	int col = index.column();
	int row = index.row();

	if (row < 0 || row >= Dates.size())
		return QVariant();

	ItemCachedData cachedData = fetchCachedData(Dates.at(row));

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (col)
			{
				case 0: return MyChat.name();
				case 1: return cachedData.title;
				case 2: return Dates.at(row).toString("dd.MM.yyyy");
				case 3: return cachedData.size;
			}

			return QVariant();
		}

		case HistoryItemRole: return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(MyChat));
		case ChatRole: return QVariant::fromValue<Chat>(MyChat);
		case DateRole: return Dates.at(row);
	}

	return QVariant();
}

void ChatDatesModel::setChat(const Chat &chat)
{
	MyChat = chat;
}

void ChatDatesModel::setDates(const QList<QDate> &dates)
{
	Cache->clear();

	beginRemoveRows(QModelIndex(), 0, Dates.size());
	Dates.clear();
	endRemoveRows();

	beginInsertRows(QModelIndex(), 0, dates.size() - 1);
	Dates = dates;
	endInsertRows();
}

QModelIndex ChatDatesModel::indexForDate(const QDate &date)
{
	return index(Dates.indexOf(date));
}
