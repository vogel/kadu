 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 #include "gui/widgets/buddy-contacts-table-item.h"
#include "gui/widgets/buddy-contacts-table-model.h"

#include "buddy-contacts-table-model-proxy.h"

BuddyContactsTableModelProxy::BuddyContactsTableModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent)
{
}

BuddyContactsTableModelProxy::~BuddyContactsTableModelProxy()
{
}

bool BuddyContactsTableModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	BuddyContactsTableModel *model = dynamic_cast<BuddyContactsTableModel *>(sourceModel());
	if (!model)
		return false;

	BuddyContactsTableItem *item = model->item(sourceRow);
	return item->action() == BuddyContactsTableItem::ItemAdd || item->action() == BuddyContactsTableItem::ItemEdit;
}
