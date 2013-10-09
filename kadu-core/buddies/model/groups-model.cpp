/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/group-manager.h"
#include "buddies/group.h"

#include "model/roles.h"

#include "protocols/protocol.h"

#include "groups-model.h"

GroupsModel::GroupsModel(QObject *parent)
	: QAbstractListModel(parent)
{
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeAdded(Group)),
			this, SLOT(groupAboutToBeAdded(Group)), Qt::DirectConnection);
	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)),
			this, SLOT(groupAdded(Group)), Qt::DirectConnection);
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupAboutToBeRemoved(Group)), Qt::DirectConnection);
	connect(GroupManager::instance(), SIGNAL(groupRemoved(Group)),
			this, SLOT(groupRemoved(Group)), Qt::DirectConnection);
}

GroupsModel::~GroupsModel()
{
	disconnect(GroupManager::instance(), 0, this, 0);
}

int GroupsModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : GroupManager::instance()->count();
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const
{
	Group grp = group(index);
	if (0 == grp)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return grp.name();
		case Qt::DecorationRole:
			return grp.icon();
		case GroupRole:
			return QVariant::fromValue(grp);
		case ItemTypeRole:
			return GroupRole;
		default:
			return QVariant();
	}
}

Group GroupsModel::group(const QModelIndex &index) const
{
	if (!index.isValid())
		return Group::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Group::null;

	return GroupManager::instance()->byIndex(index.row());
}

int GroupsModel::groupIndex(Group group) const
{
	return GroupManager::instance()->indexOf(group);
}


QModelIndexList GroupsModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const int i = groupIndex(value.value<Group>());
	if (-1 != i)
		result.append(index(i, 0));

	return result;
}

void GroupsModel::groupAboutToBeAdded(Group group)
{
	Q_UNUSED(group)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void GroupsModel::groupAdded(Group group)
{
	Q_UNUSED(group)

	endInsertRows();
}

void GroupsModel::groupAboutToBeRemoved(Group group)
{
	int index = groupIndex(group);
	beginRemoveRows(QModelIndex(), index, index);
}

void GroupsModel::groupRemoved(Group group)
{
	Q_UNUSED(group)

	endRemoveRows();
}

#include "moc_groups-model.cpp"
