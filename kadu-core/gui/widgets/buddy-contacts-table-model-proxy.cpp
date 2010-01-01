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
#include "model/roles.h"

#include "buddy-contacts-table-model-proxy.h"

BuddyContactsTableModelProxy::BuddyContactsTableModelProxy(QObject *parent) :
		QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	sort(0);
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

bool BuddyContactsTableModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	BuddyContactsTableItem *leftItem = qvariant_cast<BuddyContactsTableItem *>(left.data(BuddyContactsTableItemRole));
	BuddyContactsTableItem *rightItem = qvariant_cast<BuddyContactsTableItem *>(right.data(BuddyContactsTableItemRole));

	if (!leftItem && !rightItem)
		return false;

	if (!rightItem)
		return true;

	if (!leftItem)
		return false;

	return leftItem->itemContactPriority() < rightItem->itemContactPriority();
}
