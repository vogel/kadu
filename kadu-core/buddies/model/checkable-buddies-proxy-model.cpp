/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/roles.h"

#include "checkable-buddies-proxy-model.h"

CheckableBuddiesProxyModel::CheckableBuddiesProxyModel(QObject *parent) :
		QIdentityProxyModel(parent)
{
}

CheckableBuddiesProxyModel::~CheckableBuddiesProxyModel()
{
}

QFlags<Qt::ItemFlag> CheckableBuddiesProxyModel::flags(const QModelIndex& index) const
{
	if (BuddyRole != index.data(ItemTypeRole))
		return QIdentityProxyModel::flags(index);

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	if (buddy)
		return QIdentityProxyModel::flags(index) | Qt::ItemIsUserCheckable;

	return QIdentityProxyModel::flags(index);
}

QVariant CheckableBuddiesProxyModel::data(const QModelIndex &index, int role) const
{
	if (role != Qt::CheckStateRole)
		return QIdentityProxyModel::data(index, role);

	if (BuddyRole != index.data(ItemTypeRole))
		return QVariant();

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	return CheckedBuddies.contains(buddy) ? Qt::Checked : Qt::Unchecked;
}

bool CheckableBuddiesProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (Qt::CheckStateRole != role)
		return false;

	if (index.parent().isValid())
		return false;

	if (BuddyRole != index.data(ItemTypeRole))
		return false;

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return false;

	Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
	if (Qt::Checked == checkState)
	{
		CheckedBuddies.insert(buddy);
		emit checkedBuddiesChanged(CheckedBuddies);
		return true;
	}
	else if (Qt::Unchecked == checkState)
	{
		CheckedBuddies.remove(buddy);
		emit checkedBuddiesChanged(CheckedBuddies);
		return true;
	}

	return false;
}

BuddySet CheckableBuddiesProxyModel::checkedBuddies() const
{
	return CheckedBuddies;
}

#include "moc_checkable-buddies-proxy-model.cpp"
