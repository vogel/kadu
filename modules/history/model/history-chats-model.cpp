/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "chat/type/chat-type-manager.h"
#include "model/roles.h"
#include "icons-manager.h"

#include "model/history-type.h"
#include "history-tree-item.h"
#include "history-chats-model.h"

HistoryChatsModel::HistoryChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	triggerAllChatTypesRegistered();
}

HistoryChatsModel::~HistoryChatsModel()
{
	triggerAllChatTypesUnregistered();
}

void HistoryChatsModel::chatTypeRegistered(ChatType *chatType)
{
	if (ChatKeys.contains(chatType))
		return;

	beginInsertRows(QModelIndex(), Chats.size(), Chats.size());
	ChatKeys.append(chatType);
	Chats.insert(ChatKeys.size() - 1, QList<Chat>());
	endInsertRows();
}

void HistoryChatsModel::chatTypeUnregistered(ChatType *chatType)
{
	if (!ChatKeys.contains(chatType))
		return;

	int index = ChatKeys.indexOf(chatType);
	beginRemoveRows(QModelIndex(), index, index);
	Chats.removeAt(index);
	ChatKeys.removeAt(index);
	endRemoveRows();
}

int HistoryChatsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int HistoryChatsModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return ChatKeys.size() + 2;

	if (parent.parent().isValid())
		return 0;

	if (parent.row() < 0)
		return 0;

	if (parent.row() >= Chats.size())
	{
		switch (parent.row() - Chats.size())
		{
			case 0:
				return StatusBuddies.size();
			case 1:
				return SmsRecipients.size();
		}
		return 0;
	}

	return Chats[parent.row()].size();
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();

	return createIndex(child.internalId(), 0, -1);
}

QVariant HistoryChatsModel::chatTypeData(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= ChatKeys.count())
		return QVariant();

	ChatType *chatType = ChatKeys.at(index.row());
	switch (role)
	{
		case Qt::DisplayRole:
			return chatType->displayNamePlural();

		case Qt::DecorationRole:
			return chatType->icon();

		case ChatTypeRole:
			return QVariant::fromValue<ChatType *>(chatType);
	}

	return QVariant();
}

QVariant HistoryChatsModel::chatData(const QModelIndex &index, int role) const
{
	if (index.internalId() < 0 || index.internalId() >= Chats.size())
		return QVariant();

	const QList<Chat> &chats = Chats[index.internalId()];
	if (index.row() < 0 || index.row() >= chats.size())
		return QVariant();

	Chat chat = chats.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
			return chat.name();

		case ChatRole:
			return QVariant::fromValue<Chat>(chat);

		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(chat));
	}

	return QVariant();
}

QVariant HistoryChatsModel::statusData(const QModelIndex &index, int role) const
{
	if (!index.parent().isValid())
	{
		switch (role)
		{
			case Qt::DisplayRole:
				return tr("Statuses");
			case Qt::DecorationRole:
				return IconsManager::instance()->iconByPath("protocols/common/online");
		}
		return QVariant();
	}

	if (index.row() < 0 || index.row() >= StatusBuddies.size())
		return QVariant();

	Buddy buddy = StatusBuddies[index.row()];
	switch (role)
	{
		case Qt::DisplayRole:
			return buddy.display();
		case BuddyRole:
			return QVariant::fromValue<Buddy>(buddy);
		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(buddy));
	}
	return QVariant();
}

QVariant HistoryChatsModel::smsRecipientData(const QModelIndex &index, int role) const
{
	if (!index.parent().isValid())
	{
		switch (role)
		{
			case Qt::DisplayRole:
				return tr("SMSes");
			case Qt::DecorationRole:
				return IconsManager::instance()->iconByPath("phone");
		}
		return QVariant();
	}

	if (index.row() < 0 || index.row() >= SmsRecipients.size())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return SmsRecipients[index.row()];
		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(SmsRecipients[index.row()]));
	}
	return QVariant();
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().parent().isValid())
		return QVariant();

	int chatTypeIndex = index.parent().isValid() ? index.internalId() : index.row();
	if (chatTypeIndex < 0)
		return QVariant();

	if (chatTypeIndex >= ChatKeys.size())
	{
		switch (chatTypeIndex - ChatKeys.size())
		{
			case 0: return statusData(index, role);
			case 1: return smsRecipientData(index, role);
		}

		return QVariant();
	}

	if (index.parent().isValid())
		return chatData(index, role);
	else
		return chatTypeData(index, role);
}

void HistoryChatsModel::clearChats()
{
	int count = Chats.size();
	for (int i = 0; i < count; i++)
	{
		beginRemoveRows(index(i, 0), 0, rowCount(index(i, 0)));
		Chats[i].clear();
		endRemoveRows();
	}
}

void HistoryChatsModel::addChat(const Chat &chat)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return;

	int id = ChatKeys.indexOf(chatType);

	QModelIndex idx = index(id, 0, QModelIndex());
	int count = rowCount(idx);

	beginInsertRows(idx, count, count);
	Chats[id].append(chat);
	endInsertRows();
}

void HistoryChatsModel::setChats(const QList<Chat> &chats)
{
	clearChats();

	foreach (const Chat &chat, chats)
		addChat(chat);
}

void HistoryChatsModel::clearStatusBuddies()
{
	QModelIndex statusParent = index(Chats.size(), 0);

	beginRemoveRows(statusParent, 0, rowCount(statusParent));
	StatusBuddies.clear();
	endRemoveRows();
}

void HistoryChatsModel::clearSmsRecipients()
{
	QModelIndex statusParent = index(Chats.size() + 1, 0);

	beginRemoveRows(statusParent, 0, rowCount(statusParent));
	SmsRecipients.clear();
	endRemoveRows();
}

void HistoryChatsModel::setStatusBuddies(const QList<Buddy> &buddies)
{
	clearStatusBuddies();

	QModelIndex statusParent = index(Chats.size(), 0);

	beginInsertRows(statusParent, 0, buddies.size());
	StatusBuddies = buddies;
	endInsertRows();
}

void HistoryChatsModel::setSmsRecipients(const QList<QString> &smsRecipients)
{
	clearSmsRecipients();

	QModelIndex statusParent = index(Chats.size() + 1, 0);

	beginInsertRows(statusParent, 0, smsRecipients.size());
	SmsRecipients = smsRecipients;
	endInsertRows();
}

QModelIndex HistoryChatsModel::chatTypeIndex(ChatType *type) const
{
	int row = ChatKeys.indexOf(type);
	if (row < 0)
		return QModelIndex();

	return index(row, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::chatIndex(const Chat &chat) const
{
	QString typeName = chat.type();
	ChatType *chatType = ChatTypeManager::instance()->chatType(typeName);
	if (!chatType)
		return QModelIndex();

	if (!ChatKeys.contains(chatType))
		return QModelIndex();

	QModelIndex typeIndex = chatTypeIndex(chatType);
	if (!typeIndex.isValid())
		return QModelIndex();

	int row = Chats[typeIndex.row()].indexOf(chat);
	return index(row, 0, typeIndex);
}

QModelIndex HistoryChatsModel::statusIndex() const
{
	return index(ChatKeys.size(), 0, QModelIndex());
}

QModelIndex HistoryChatsModel::statusBuddyIndex(const Buddy &buddy) const
{
	QModelIndex parent = statusIndex();
	if (!parent.isValid())
		return QModelIndex();

	int row = StatusBuddies.indexOf(buddy);
	return index(row, 0, parent);
}

QModelIndex HistoryChatsModel::smsIndex() const
{
	return index(ChatKeys.size() + 1, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::smsRecipientIndex(const QString &recipient) const
{
	QModelIndex parent = smsIndex();
	if (!parent.isValid())
		return QModelIndex();

	int row = SmsRecipients.indexOf(recipient);
	return index(row, 0, parent);
}
