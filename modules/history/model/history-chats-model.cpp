/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "contacts/model/contacts-model-base.h"

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
	const QList<ChatType> &chatTypes = ChatTypeManager::instance()->chatTypes();

	if (!parent.isValid())
		return chatTypes.size();

	if (parent.parent().isValid())
		return 0;

	if (parent.row() < 0 || parent.row() >= chatTypes.size())
		return 0;

	const ChatType &chatType = chatTypes.at(parent.row());
	if (!Chats.contains(chatType))
		return 0;

	return Chats.value(chatType).size();
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column, -1); // ROOT

	return createIndex(row, column, parent.row());
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();

	return createIndex(child.internalId(), 0, -1);
}

QVariant HistoryChatsModel::chatTypeData(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= ChatTypeManager::instance()->chatTypes().count())
		return QVariant();
	
	ChatType chatType = ChatTypeManager::instance()->chatTypes().at(index.row());
	switch (role)
	{
		case Qt::DisplayRole:
			return chatType.displayName();

		case Qt::DecorationRole:
			return chatType.icon();

		case ChatTypeRole:
			return QVariant::fromValue<ChatType>(chatType);
	}
	
	return QVariant();
}

QVariant HistoryChatsModel::chatData(const QModelIndex &index, int role) const
{
	const QList<ChatType> &chatTypes = ChatTypeManager::instance()->chatTypes();
	if (index.internalId() < 0 || index.internalId() >= chatTypes.size())
		return QVariant();

	const ChatType &chatType = chatTypes.at(index.internalId());
	const QList<Chat *> chats = Chats.value(chatType);
	if (index.row() < 0 || index.row() >= chats.size())
		return QVariant();

	Chat *chat = chats.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
			return chat->name();

		case ChatRole:
			return QVariant::fromValue<Chat *>(chat);
	}

	return QVariant();
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid())
		return chatData(index, role);
	else
		return chatTypeData(index, role);
}

void HistoryChatsModel::clear()
{
	beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1);

	Chats = QMap<ChatType, QList<Chat *> >();

	endRemoveRows();
}

void HistoryChatsModel::addChat(Chat *chat)
{
	if (!Chats.contains(chat->type()))
	{
		beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
		Chats.insert(chat->type(), QList<Chat *>());
		endInsertRows();
	}

	int id = Chats.keys().indexOf(chat->type());
	QModelIndex idx = index(id, 0, QModelIndex());
	int count = rowCount(idx);
	beginInsertRows(idx, count, count);
	Chats[chat->type()].append(chat);
	endInsertRows();
}

QModelIndex HistoryChatsModel::chatTypeIndex(ChatType type) const
{
	int row = ChatTypeManager::instance()->chatTypes().indexOf(type);
	if (row < 0)
		return QModelIndex();

	return index(row, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::chatIndex(Chat *chat) const
{
	if (!Chats.contains(chat->type()))
		return QModelIndex();

	QModelIndex typeIndex = chatTypeIndex(chat->type());
	if (!typeIndex.isValid())
		return QModelIndex();

	int row = Chats.value(chat->type()).indexOf(chat);
	return index(row, 0, typeIndex);
}
