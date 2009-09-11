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

class HistoryChatsModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public:
	HistoryChatsModelProxy(QObject *parent = 0);

};

#endif // HISTORY_CHATS_MODEL_PROXY
