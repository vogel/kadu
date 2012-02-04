/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-list-mime-data-helper.h"
#include "chat/model/chat-data-extractor.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-data-extractor.h"
#include "model/roles.h"
#include "talkable/talkable.h"

#include "chats-list-model.h"

ChatsListModel::ChatsListModel(QObject *parent) :
		QAbstractItemModel(parent)
{
}

ChatsListModel::~ChatsListModel()
{
}

void ChatsListModel::setChats(const QVector<Chat> &chats)
{
	beginResetModel();

	foreach (const Chat &chat, Chats)
		disconnect(chat, SIGNAL(updated()), this, SLOT(chatUpdated()));

	Chats = chats;

	// We want all items to be loaded
	foreach (const Chat &chat, Chats)
	{
		Q_ASSERT(chat.data());
		chat.data()->ensureLoaded();
		connect(chat, SIGNAL(updated()), this, SLOT(chatUpdated()));
	}

	endResetModel();
}

QModelIndex ChatsListModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

int ChatsListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int ChatsListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.parent().isValid())
		return 0;

	if (parent.isValid())
	{
		const Chat &chat = chatAt(parent);
		return chat.contacts().size();
	}

	return Chats.size();
}

QFlags<Qt::ItemFlag> ChatsListModel::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QModelIndex ChatsListModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();
	else
		return index(child.internalId(), 0, QModelIndex());
}

QVariant ChatsListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QModelIndex parentIndex = parent(index);
	if (!parentIndex.isValid())
	{
		if (ItemTypeRole == role)
			return ChatRole;

		return ChatDataExtractor::data(chatAt(index), role);
	}

	if (!parentIndex.parent().isValid())
	{

		const Chat &chat = chatAt(parentIndex);
		const QList<Contact> &contacts = chat.contacts().toList();

		if (index.row() >= contacts.size())
			return QVariant();

		return ContactDataExtractor::data(contacts.at(index.row()), role, true);
	}

	return QVariant();
}

void ChatsListModel::chatUpdated()
{
	Chat chat(sender());
	if (!chat)
		return;

	const QModelIndexList &indexes = indexListForValue(chat);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (index.isValid())
		emit dataChanged(index, index);
}

Chat ChatsListModel::chatAt(const QModelIndex &index) const
{
	const QModelIndex &parent = index.parent();
	const int row = parent.isValid() ? parent.row() : index.row();

	return row >= 0 && row < Chats.size() ? Chats.at(row) : Chat::null;
}

Chat ChatsListModel::chatFromVariant(const QVariant &variant) const
{
	const Chat &chat = variant.value<Chat>();

	if (chat)
		return chat;

	const Talkable &talkable = variant.value<Talkable>();
	if (talkable.isValidChat())
		return talkable.toChat();

	return Chat::null;
}

QModelIndexList ChatsListModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Chat &chat = chatFromVariant(value);

	if (chat)
	{
		const int i = Chats.indexOf(chat);
		if (-1 != i)
			result.append(index(i, 0));
		return result;
	}

	const Contact &contact = value.value<Contact>();
	if (contact)
	{
		const int count = Chats.count();

		for (int i = 0; i < count; i++)
		{
			const Chat &chat = Chats.at(i);
			const QList<Contact> &contacts = chat.contacts().toList();
			const int contactIndex = contacts.indexOf(contact);

			if (-1 != contactIndex)
				result.append(index(i, 0).child(contactIndex, 0));
		}

		return result;
	}

	return result;
}

// D&D

QStringList ChatsListModel::mimeTypes() const
{
	return ChatListMimeDataHelper::mimeTypes();
}

QMimeData * ChatsListModel::mimeData(const QModelIndexList &indexes) const
{
	QList<Chat> list;
	foreach (const QModelIndex &index, indexes)
	{
		Chat chat = index.data(ChatRole).value<Chat>();
		if (chat)
			list << chat;
	}

	return ChatListMimeDataHelper::toMimeData(list);
}
