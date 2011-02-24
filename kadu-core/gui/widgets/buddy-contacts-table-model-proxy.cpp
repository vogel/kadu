/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
	Q_UNUSED(sourceParent)

	BuddyContactsTableModel *model = qobject_cast<BuddyContactsTableModel *>(sourceModel());
	if (!model)
		return false;

	BuddyContactsTableItem *item = model->item(sourceRow);
	return (!item->itemAccount() || item->itemAccount().details()) &&
			(item->action() == BuddyContactsTableItem::ItemAdd || item->action() == BuddyContactsTableItem::ItemEdit);
}

bool BuddyContactsTableModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	BuddyContactsTableItem *leftItem = left.data(BuddyContactsTableItemRole).value<BuddyContactsTableItem *>();
	BuddyContactsTableItem *rightItem = right.data(BuddyContactsTableItemRole).value<BuddyContactsTableItem *>();

	if (!leftItem && !rightItem)
		return false;

	if (!rightItem)
		return true;

	if (!leftItem)
		return false;

	return leftItem->itemContactPriority() < rightItem->itemContactPriority();
}
