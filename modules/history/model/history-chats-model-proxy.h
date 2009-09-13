 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_CHATS_MODEL_PROXY
#define HISTORY_CHATS_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

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

	QModelIndex chatTypeIndex(ChatType type) const;
	QModelIndex chatIndex(Chat *chat) const;

};

#endif // HISTORY_CHATS_MODEL_PROXY
