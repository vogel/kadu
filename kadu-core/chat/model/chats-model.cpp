/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/chat-manager.h"
#include "chat/model/chat-data-extractor.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-data-extractor.h"
#include "model/roles.h"

#include "chats-model.h"

ChatsModel::ChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	ChatManager *manager = ChatManager::instance();

	// We want all items to be loaded. Needed to fix bug #2383.
	foreach (const Chat &chat, manager->allItems())
	{
		Q_ASSERT(chat.data());
		chat.data()->ensureLoaded();
	}

	connect(manager, SIGNAL(chatAboutToBeAdded(Chat)),
			this, SLOT(chatAboutToBeAdded(Chat)), Qt::DirectConnection);
	connect(manager, SIGNAL(chatAdded(Chat)),
			this, SLOT(chatAdded(Chat)), Qt::DirectConnection);
	connect(manager, SIGNAL(chatAboutToBeRemoved(Chat)),
			this, SLOT(chatAboutToBeRemoved(Chat)), Qt::DirectConnection);
	connect(manager, SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)), Qt::DirectConnection);
	connect(manager, SIGNAL(chatUpdated(const Chat &)),
			this, SLOT(chatUpdated(const Chat &)), Qt::DirectConnection);
}

ChatsModel::~ChatsModel()
{
	ChatManager *manager = ChatManager::instance();
	disconnect(manager, SIGNAL(chatAboutToBeAdded(Chat)),
			this, SLOT(chatAboutToBeAdded(Chat)));
	disconnect(manager, SIGNAL(chatAdded(Chat)),
			this, SLOT(chatAdded(Chat)));
	disconnect(manager, SIGNAL(chatAboutToBeRemoved(Chat)),
			this, SLOT(chatAboutToBeRemoved(Chat)));
	disconnect(manager, SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)));
	disconnect(manager, SIGNAL(chatUpdated(const Chat &)),
			this, SLOT(chatUpdated(const Chat &)));
}

QModelIndex ChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

int ChatsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int ChatsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.parent().isValid())
		return 0;

	if (parent.isValid())
	{
		const Chat &chat = chatAt(parent);
		return chat.contacts().size();
	}

	return ChatManager::instance()->count();
}

QFlags<Qt::ItemFlag> ChatsModel::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QModelIndex ChatsModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();
	else
		return index(child.internalId(), 0, QModelIndex());
}

QVariant ChatsModel::data(const QModelIndex &index, int role) const
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

void ChatsModel::chatAboutToBeAdded(Chat chat)
{
	Q_UNUSED(chat)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void ChatsModel::chatAdded(Chat chat)
{
	endInsertRows();

	// force refresh on proxy sorring model
	// setDynamicSortFilter does not work properly when adding/removing items, only when changing item data
	// this is Qt bug
	// see bug #2167

	const QModelIndexList &indexes = indexListForValue(chat);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	emit dataChanged(index, index);
}

void ChatsModel::chatAboutToBeRemoved(Chat chat)
{
	const QModelIndexList &indexes = indexListForValue(chat);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	beginRemoveRows(QModelIndex(), index.row(), index.row());
}

void ChatsModel::chatRemoved(Chat chat)
{
	Q_UNUSED(chat)

	endRemoveRows();
}

void ChatsModel::chatUpdated(const Chat &chat)
{
	const QModelIndexList &indexes = indexListForValue(chat);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (index.isValid())
		emit dataChanged(index, index);
}

Chat ChatsModel::chatAt(const QModelIndex &index) const
{
	const QModelIndex &parent = index.parent();
	const int row = parent.isValid() ? parent.row() : index.row();

	return ChatManager::instance()->byIndex(row);
}

QModelIndexList ChatsModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Chat &chat = value.value<Chat>();

	if (chat)
	{
		const int i = ChatManager::instance()->indexOf(chat);
		if (-1 != i)
			result.append(index(i, 0));
		return result;
	}

	const Contact &contact = value.value<Contact>();
	if (contact)
	{
		const QVector<Chat> &chats = ChatManager::instance()->items();
		const int count = chats.count();

		for (int i = 0; i < count; i++)
		{
			const Chat &chat = chats.at(i);
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

QStringList ChatsModel::mimeTypes() const
{
	return ChatListMimeDataHelper::mimeTypes();
}

QMimeData * ChatsModel::mimeData(const QModelIndexList &indexes) const
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
