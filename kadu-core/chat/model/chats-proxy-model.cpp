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

#include "chat/filter/chat-filter.h"
#include "chat/model/chats-model.h"
#include "model/roles.h"

#include "chats-proxy-model.h"

ChatsProxyModel::ChatsProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent), SourceChatsModel(0)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
}

ChatsProxyModel::~ChatsProxyModel()
{

}

void ChatsProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QAbstractItemModel *oldModel = dynamic_cast<QAbstractItemModel *>(SourceChatsModel);
	if (oldModel)
		disconnect(oldModel, SIGNAL(destroyed()), this, SLOT(modelDestroyed()));

	SourceChatsModel = dynamic_cast<ChatsModel *>(sourceModel);
	QSortFilterProxyModel::setSourceModel(sourceModel);

	if (sourceModel)
		connect(sourceModel, SIGNAL(destroyed()), this, SLOT(modelDestroyed()));
}

int ChatsProxyModel::compareNames(const QString &n1, const QString &n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

void ChatsProxyModel::modelDestroyed()
{
	SourceChatsModel = 0;
	QSortFilterProxyModel::setSourceModel(0);
}

bool ChatsProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	const Chat &leftChat = left.data(ChatRole).value<Chat>();
	const Chat &rightChat = right.data(ChatRole).value<Chat>();

	if (leftChat.unreadMessagesCount() > 0 && rightChat.unreadMessagesCount() == 0)
		return -1;
	if (leftChat.unreadMessagesCount() == 0 && rightChat.unreadMessagesCount() > 0)
		return 1;

	const QString &leftChatDisplay = left.data().toString();
	const QString &rightChatDisplay = right.data().toString();

	int displayCompare = compareNames(leftChatDisplay, rightChatDisplay);
	return displayCompare < 0;
}

bool ChatsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (sourceParent.isValid())
		return true;

	const Chat &chat = SourceChatsModel->chatAt(sourceModel()->index(sourceRow, 0, sourceParent));
	foreach (ChatFilter *filter, Filters)
		if (!filter->acceptChat(chat))
			return false;

	return true;
}

void ChatsProxyModel::addFilter(ChatFilter *filter)
{
	if (Filters.contains(filter))
		return;

	Filters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void ChatsProxyModel::removeFilter(ChatFilter *filter)
{
	if (Filters.removeAll(filter) <= 0)
		return;

	invalidateFilter();
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}
