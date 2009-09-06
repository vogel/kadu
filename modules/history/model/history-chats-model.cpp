/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "chat/type/chat-type-manager.h"

#include "history-chats-model.h"

HistoryChatsModel::HistoryChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
}

HistoryChatsModel::~HistoryChatsModel()
{
}

int HistoryChatsModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int HistoryChatsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return ChatTypeManager::instance()->chatTypes().count();
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, 0);
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid())
		return QVariant();

	if (index.row() < 0 || index.row() >= ChatTypeManager::instance()->chatTypes().count())
		return QVariant();

	ChatType chatType = ChatTypeManager::instance()->chatTypes().at(index.row());
	switch (role)
	{
		case Qt::DisplayRole:
			return chatType.displayName();

		case Qt::DecorationRole:
			return chatType.icon();
	}

	return QVariant();
}
