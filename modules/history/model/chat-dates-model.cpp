/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "chat/chat.h"
#include "chat/message/message.h"

#include "history.h"

#include "chat-dates-model.h"
#include <chat/message/formatted-message.h>

ChatDatesModel::ChatDatesModel(Chat *chat, QList<QDate> dates, QObject *parent) :
		QAbstractListModel(parent), MyChat(chat), Dates(dates)
{
}

ChatDatesModel::~ChatDatesModel()
{
}

int ChatDatesModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

int ChatDatesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return Dates.size();
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

QString ChatDatesModel::fetchTitle(QDate date) const
{
	QList<Message> messages = History::instance()->getMessages(MyChat, date, 1);
	if (messages.size() == 0)
		return "";

	Message firstMessage = messages.first();
	FormattedMessage formatted = FormattedMessage(firstMessage.content());
	QString title = formatted.toPlain();

	if (title.length() > 20)
	{
		title.truncate(20);
		title += " ...";
	}

	return title;
}

int ChatDatesModel::fetchSize(QDate date) const
{
	return History::instance()->getMessagesCount(MyChat, date);
}

ChatDatesModel::ItemCachedData ChatDatesModel::fetchCachedData(QDate date) const
{
	if (Cache.contains(date))
		return Cache.value(date);

	ItemCachedData cache;
	cache.title = fetchTitle(date);
	cache.size = fetchSize(date);
// TODO: cant do that, need another place
// 	Cache.insert(date, cache);

	return cache;
}

QVariant ChatDatesModel::data(const QModelIndex &index, int role) const
{
	if (!MyChat || role != Qt::DisplayRole)
		return QVariant();

	int col = index.column();
	int row = index.row();

	if (row < 0 || row >= Dates.size())
		return QVariant();

	ItemCachedData cachedData = fetchCachedData(Dates.at(row));

	switch (col)
	{
		case 0: return MyChat->name();
		case 1: return cachedData.title;
		case 2: return Dates.at(row).toString("dd.MM.yyyy");
		case 3: return cachedData.size;
	}
}

void ChatDatesModel::setChat(Chat *chat)
{
	MyChat = chat;
}

void ChatDatesModel::setDates(QList<QDate> dates)
{
	Cache.clear();

	beginRemoveRows(QModelIndex(), 0, Dates.size());
	Dates.clear();
	endRemoveRows();

	beginInsertRows(QModelIndex(), 0, dates.size());
	Dates = dates;
	endInsertRows();
}
