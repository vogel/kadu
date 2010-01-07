 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/group.h"
#include "buddies/group-manager.h"

#include "model/roles.h"

#include "protocols/protocol.h"

#include "groups-model.h"

GroupsModel::GroupsModel(QObject *parent)
	: QAbstractListModel(parent)
{
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeAdded(Group)),
			this, SLOT(groupAboutToBeAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)),
			this, SLOT(groupAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupAboutToBeRemoved(Group)));
	connect(GroupManager::instance(), SIGNAL(groupRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
}

GroupsModel::~GroupsModel()
{
	disconnect(GroupManager::instance(), SIGNAL(groupAboutToBeAdded(Group)),
			this, SLOT(groupAboutToBeAdded(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupAdded(Group)),
			this, SLOT(groupAdded(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupAboutToBeRemoved(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
}

int GroupsModel::rowCount(const QModelIndex &parent) const
{
	return GroupManager::instance()->count();
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
		default:
			return QVariant();
	}
}

QVariant GroupsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Group GroupsModel::group(const QModelIndex &index) const
{
	if (!index.isValid())
		return Group::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Group::null;

	return GroupManager::instance()->byIndex(index.row());
}

int GroupsModel::groupIndex(Group group)
{
	return GroupManager::instance()->indexOf(group);
}


QModelIndex GroupsModel::groupModelIndex(Group group)
{
	return createIndex(groupIndex(group), 0, 0);
}

void GroupsModel::groupAboutToBeAdded(Group group)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void GroupsModel::groupAdded(Group group)
{
	endInsertRows();
}

void GroupsModel::groupAboutToBeRemoved(Group group)
{
	int index = groupIndex(group);
	beginRemoveRows(QModelIndex(), index, index);
}

void GroupsModel::groupRemoved(Group group)
{
	endRemoveRows();
}
