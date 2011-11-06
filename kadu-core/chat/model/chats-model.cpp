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

#include "chat/model/chat-data-extractor.h"
#include "chat/chat-manager.h"
#include "contacts/model/contact-data-extractor.h"
#include "contacts/contact-set.h"
#include "model/roles.h"

#include "chats-model.h"

ChatsModel::ChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	ChatManager *manager = ChatManager::instance();
	connect(manager, SIGNAL(chatAboutToBeAdded(Chat)),
			this, SLOT(chatAboutToBeAdded(Chat)));
	connect(manager, SIGNAL(chatAdded(Chat)),
			this, SLOT(chatAdded(Chat)));
	connect(manager, SIGNAL(chatAboutToBeRemoved(Chat)),
			this, SLOT(chatAboutToBeRemoved(Chat)));
	connect(manager, SIGNAL(chatRemoved(Chat)),
			this, SLOT(chatRemoved(Chat)));
	connect(manager, SIGNAL(chatUpdated(const Chat &)),
			this, SLOT(chatUpdated(const Chat &)));
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
	QModelIndex index = indexForValue(chat);
	emit dataChanged(index, index);
}

void ChatsModel::chatAboutToBeRemoved(Chat chat)
{
	int index = indexForValue(chat).row();
	beginRemoveRows(QModelIndex(), index, index);
}

void ChatsModel::chatRemoved(Chat chat)
{
	Q_UNUSED(chat)

	endRemoveRows();
}

void ChatsModel::chatUpdated(const Chat &chat)
{
	QModelIndex index = indexForValue(chat);
	if (index.isValid())
		emit dataChanged(index, index);
}

Chat ChatsModel::chatAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	int row = parent.isValid() ? parent.row() : index.row();

	return ChatManager::instance()->byIndex(row);
}

QModelIndex ChatsModel::indexForValue(const QVariant &value) const
{
	const Chat &chat = value.value<Chat>();
	if (!chat)
		return QModelIndex();

	int result = ChatManager::instance()->indexOf(chat);

	return index(result, 0);
}
