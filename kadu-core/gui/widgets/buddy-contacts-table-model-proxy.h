 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_CONTACTS_TABLE_MODEL_PROXY_H
#define BUDDY_CONTACTS_TABLE_MODEL_PROXY_H

#include <QtGui/QSortFilterProxyModel>

class BuddyContactsTableModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit BuddyContactsTableModelProxy(QObject *parent = 0);
	virtual ~BuddyContactsTableModelProxy();

};

#endif // BUDDY_CONTACTS_TABLE_MODEL_PROXY_H
