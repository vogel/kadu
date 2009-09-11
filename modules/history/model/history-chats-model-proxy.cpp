 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/type/chat-type.h"
#include "chat/chat.h"
#include "contacts/model/contacts-model-base.h"

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

bool HistoryChatsModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	// chats?
	Chat *leftChat = left.data(ChatRole).value<Chat *>();
	Chat *rightChat = right.data(ChatRole).value<Chat *>();

	if (leftChat && rightChat)
		return compareNames(leftChat->name(), rightChat->name()) < 0;

	ChatType leftType = left.data(ChatTypeRole).value<ChatType>();
	ChatType rightType = right.data(ChatTypeRole).value<ChatType>();

	return compareNames(leftType.displayName(), rightType.displayName()) < 0;
}
