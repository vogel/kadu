/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QAction>

#include "model/roles.h"

#include "action-list-model.h"

ActionListModel::ActionListModel(QObject *parent) :
		QAbstractItemModel(parent)
{
}

ActionListModel::~ActionListModel()
{
}

void ActionListModel::setActionList(const QList<QAction *> &actionList)
{
	beginResetModel();
	ActionList = actionList;
	endResetModel();
}

void ActionListModel::appendAction(QAction *action)
{
	const int count = ActionList.count();

	beginInsertRows(QModelIndex(), count, count);
	ActionList.append(action);
	endInsertRows();
}

void ActionListModel::insertAction(int index, QAction *action)
{
	if (index < 0)
		index = 0;
	if (index > ActionList.count())
		index = ActionList.count();

	beginInsertRows(QModelIndex(), index, index);
	ActionList.insert(index, action);
	endInsertRows();
}

void ActionListModel::removeAction(QAction *action)
{
	int index = ActionList.indexOf(action);
	if (index < 0)
		return;

	beginRemoveRows(QModelIndex(), index, index);
	ActionList.removeAt(index);
	endRemoveRows();
}

QModelIndex ActionListModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent)
	        ? createIndex(row, column, parent.isValid() ? parent.row() : -1)
	        : QModelIndex();
}

QModelIndex ActionListModel::parent(const QModelIndex &child) const
{
	Q_UNUSED(child)

	return QModelIndex();
}

int ActionListModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int ActionListModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : ActionList.size();
}

Qt::ItemFlags ActionListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemFlags();

	QAction *action = ActionList.at(index.row());
	if (!action || action->isSeparator())
		return Qt::ItemFlags();

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ActionListModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid())
		return QVariant();

	if (index.row() < 0 || index.row() >= ActionList.size())
		return QVariant();

	QAction *action = ActionList.at(index.row());
	if (!action)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return action->text();

		case Qt::DecorationRole:
			return action->icon();

		case Qt::FontRole:
			return action->font();

		case ActionRole:
			return QVariant::fromValue<QAction *>(action);

		case ItemTypeRole:
			return ActionRole;
	}

	return QVariant();
}

QModelIndexList ActionListModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	QAction *action = value.value<QAction *>();
	int row = ActionList.indexOf(action);

	if (row >= 0)
		result.append(index(row, 0));

	return result;
}

#include "moc_action-list-model.cpp"
