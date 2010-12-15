/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_CHATS_MODEL_PROXY
#define HISTORY_CHATS_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

class Buddy;
class Chat;
class ChatFilter;
class ChatType;
class HistoryChatsModel;

class HistoryChatsModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

	HistoryChatsModel *Model;
	QList<ChatFilter *> Filters;

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	HistoryChatsModelProxy(QObject *parent = 0);

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void addFilter(ChatFilter *filter);
	void removeFilter(ChatFilter *filter);

	QModelIndex chatTypeIndex(ChatType *type) const;
	QModelIndex chatIndex(const Chat &chat) const;

	QModelIndex statusIndex() const;
	QModelIndex statusBuddyIndex(const Buddy &buddy) const;

	QModelIndex smsIndex() const;
	QModelIndex smsRecipientIndex(const QString &smsRecipient) const;

};

#endif // HISTORY_CHATS_MODEL_PROXY
