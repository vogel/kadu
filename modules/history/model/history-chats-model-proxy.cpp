 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/filter/chat-filter.h"
#include "chat/type/chat-type.h"
#include "chat/chat.h"
#include "model/roles.h"
#include "modules/history/model/history-chats-model.h"

#include "history-chats-model-proxy.h"

HistoryChatsModelProxy::HistoryChatsModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

int HistoryChatsModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
			? n1.toLower().localeAwareCompare(n2.toLower())
			: n1.localeAwareCompare(n2);
}

bool HistoryChatsModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (0 == Filters.size())
		return true;

	QModelIndex sourceChild = sourceParent.child(sourceRow, 0);
	Chat chat = sourceChild.data(ChatRole).value<Chat >();
	if (!chat)
		return true;

	foreach (ChatFilter *filter, Filters)
		if (!filter->acceptChat(chat))
			return false;

	return true;
}

bool HistoryChatsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	// chats?
	Chat leftChat = left.data(ChatRole).value<Chat >();
	Chat rightChat = right.data(ChatRole).value<Chat >();

	if (!leftChat.isNull() && !rightChat.isNull())
		return compareNames(leftChat.name(), rightChat.name()) < 0;

	ChatType *leftType = left.data(ChatTypeRole).value<ChatType *>();
	ChatType *rightType = right.data(ChatTypeRole).value<ChatType *>();

	QString leftName = leftType ? leftType->displayName() : "";
	QString rightName = rightType ? rightType->displayName() : "";

	return compareNames(leftName, rightName) < 0;
}

void HistoryChatsModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);
	Model = dynamic_cast<HistoryChatsModel *>(sourceModel);
}

void HistoryChatsModelProxy::addFilter(ChatFilter *filter)
{
	if (!filter)
		return;

	Filters.append(filter);
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

void HistoryChatsModelProxy::removeFilter(ChatFilter *filter)
{
	if (!filter)
		return;
	
	Filters.removeAll(filter);
	disconnect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));

	invalidateFilter();
}

QModelIndex HistoryChatsModelProxy::chatTypeIndex(ChatType *type) const
{
	if (!Model)
		return QModelIndex();

	QModelIndex index = Model->chatTypeIndex(type);
	return mapFromSource(index);
}

QModelIndex HistoryChatsModelProxy::chatIndex(Chat chat) const
{
	if (!Model)
		return QModelIndex();
	
	QModelIndex index = Model->chatIndex(chat);
	return mapFromSource(index);
}
