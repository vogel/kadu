/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtGui/QFont>

#include "model/roles.h"

#include "actions-proxy-model.h"

ActionsProxyModel::ActionsProxyModel(QObject *parent) :
		QAbstractProxyModel(parent)
{
}

ActionsProxyModel::~ActionsProxyModel()
{
}

void ActionsProxyModel::updateVisibleActions(QList<QAction *> &visibleActions, const QList<ActionWithVisibility> &actions, int globalPosition)
{
	int i = 0, sourceModelRowCount = sourceModel() ? sourceModel()->rowCount() : 0;

	foreach (const ActionWithVisibility &action, actions)
	{
		// whether the action should be visible
		if (!(sourceModelRowCount == 0 && action.Visibility & NotVisibleWithEmptySourceModel) &&
			!(sourceModelRowCount == 1 && action.Visibility & NotVisibleWithOneRowSourceModel))
		{
			// if it should, but it isn't yet
			if (!visibleActions.contains(action.Action))
			{
				beginInsertRows(QModelIndex(), globalPosition, globalPosition);
				visibleActions.insert(i, action.Action);
				endInsertRows();
			}
			i++;
			globalPosition++;
		}
		// if it shouldn't, but now is visible
		else if (visibleActions.contains(action.Action))
		{
			beginRemoveRows(QModelIndex(), globalPosition, globalPosition);
			visibleActions.removeAt(i);
			endRemoveRows();
		}
	}
}

void ActionsProxyModel::updateVisibleBeforeActions()
{
	updateVisibleActions(VisibleBeforeActions, BeforeActions, 0);
}

void ActionsProxyModel::updateVisibleAfterActions()
{
	updateVisibleActions(VisibleAfterActions, AfterActions,
			VisibleBeforeActions.count() + (sourceModel() ? sourceModel()->rowCount() : 0));
}

void ActionsProxyModel::addBeforeAction(QAction *action, ActionVisibility actionVisibility)
{
	ActionWithVisibility a(action, actionVisibility);
	BeforeActions.append(a);

	updateVisibleBeforeActions();
}

void ActionsProxyModel::addAfterAction(QAction *action, ActionVisibility actionVisibility)
{
	ActionWithVisibility a(action, actionVisibility);
	AfterActions.append(a);

	updateVisibleAfterActions();
}

void ActionsProxyModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
	QAbstractItemModel *currentModel = sourceModel();
	if (currentModel)
	{
		disconnect(currentModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
				this, SLOT(sourceDataChanged(const QModelIndex &, const QModelIndex &)));
		disconnect(currentModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
				this, SLOT(sourceHeaderDataChanged(Qt::Orientation, int, int)));
		disconnect(currentModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsAboutToBeInserted(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsInserted(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsAboutToBeInserted(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsInserted(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsRemoved(const QModelIndex &, int, int)));
		disconnect(currentModel, SIGNAL(layoutAboutToBeChanged()),
				this, SLOT(sourceLayoutAboutToBeChanged()));
		disconnect(currentModel, SIGNAL(layoutChanged()),
				this, SLOT(sourceLayoutChanged()));
	}

	QAbstractProxyModel::setSourceModel(newSourceModel);

	if (newSourceModel)
	{
		connect(newSourceModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
				this, SLOT(sourceDataChanged(const QModelIndex &, const QModelIndex &)));
		connect(newSourceModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
				this, SLOT(sourceHeaderDataChanged(Qt::Orientation, int, int)));
		connect(newSourceModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsAboutToBeInserted(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsInserted(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsAboutToBeInserted(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsInserted(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
				this, SLOT(sourceColumnsRemoved(const QModelIndex &, int, int)));
		connect(newSourceModel, SIGNAL(layoutAboutToBeChanged()),
				this, SLOT(sourceLayoutAboutToBeChanged()));
		connect(newSourceModel, SIGNAL(layoutChanged()),
				this, SLOT(sourceLayoutChanged()));
	}

	updateVisibleBeforeActions();
	updateVisibleAfterActions();
}

void ActionsProxyModel::sourceDataChanged(const QModelIndex &sourceTopLeft, const QModelIndex &sourceBottomRight)
{
	emit dataChanged(mapFromSource(sourceTopLeft), mapFromSource(sourceBottomRight));
}

void ActionsProxyModel::sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end)
{
	emit headerDataChanged(orientation, start, end);
}

void ActionsProxyModel::sourceRowsAboutToBeInserted(const QModelIndex &sourceParent, int start, int end)
{
	if (!sourceParent.isValid())
	{
		start += VisibleBeforeActions.size();
		end += VisibleBeforeActions.size();
	}

	beginInsertRows(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceRowsInserted(const QModelIndex &sourceParent, int start, int end)
{
	Q_UNUSED(sourceParent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	endInsertRows();
	updateVisibleBeforeActions();
	updateVisibleAfterActions();
}

void ActionsProxyModel::sourceColumnsAboutToBeInserted(const QModelIndex &sourceParent, int start, int end)
{
	beginInsertColumns(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceColumnsInserted(const QModelIndex &sourceParent, int start, int end)
{
	Q_UNUSED(sourceParent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	endInsertColumns();
}

void ActionsProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &sourceParent, int start, int end)
{
	if (!sourceParent.isValid())
	{
		start += VisibleBeforeActions.size();
		end += VisibleBeforeActions.size();
	}

	beginRemoveRows(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceRowsRemoved(const QModelIndex &sourceParent, int start, int end)
{
	Q_UNUSED(sourceParent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	endRemoveRows();
	updateVisibleBeforeActions();
	updateVisibleAfterActions();
}

void ActionsProxyModel::sourceColumnsAboutToBeRemoved(const QModelIndex &sourceParent, int start, int end)
{
	beginRemoveColumns(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceColumnsRemoved(const QModelIndex &sourceParent, int start, int end)
{
	Q_UNUSED(sourceParent)
	Q_UNUSED(start)
	Q_UNUSED(end)

	endRemoveColumns();
}

void ActionsProxyModel::sourceLayoutAboutToBeChanged()
{
	emit layoutAboutToBeChanged();
}

void ActionsProxyModel::sourceLayoutChanged()
{
	emit layoutChanged();
}

int ActionsProxyModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : sourceModel()->columnCount(mapToSource(parent));
}

int ActionsProxyModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	if (sourceModel())
		return sourceModel()->rowCount(mapToSource(parent)) + VisibleBeforeActions.count() + VisibleAfterActions.count();
	else
		return VisibleBeforeActions.count() + VisibleAfterActions.count();
}

QModelIndex ActionsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
}

QModelIndex ActionsProxyModel::parent(const QModelIndex &index) const
{
	Q_UNUSED(index)

	return QModelIndex(); // no parent, only plain list
}

QAction * ActionsProxyModel::actionForIndex(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	int beforeIndex = index.row();
	int afterIndex = index.row() - VisibleBeforeActions.count() - sourceModel()->rowCount();

	if (beforeIndex >= 0 && beforeIndex < VisibleBeforeActions.count())
		return VisibleBeforeActions[beforeIndex];
	else if (afterIndex >= 0 && afterIndex < VisibleAfterActions.count())
		return VisibleAfterActions[afterIndex];

	return 0;
}

Qt::ItemFlags ActionsProxyModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemFlags();

	QModelIndex sourceIndex = mapToSource(index);
	if (sourceIndex.isValid())
		return sourceModel()->flags(sourceIndex);

	QAction *action = actionForIndex(index);
	if (!action || action->isSeparator())
		return Qt::ItemFlags();

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ActionsProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
	if (!proxyIndex.isValid())
		return QVariant();

	QAction *action = actionForIndex(proxyIndex);

	if (!action)
		return sourceModel()->data(mapToSource(proxyIndex), role);

	switch (role)
	{
		case Qt::DisplayRole:
			return action->text();

		case Qt::DecorationRole:
			return action->icon();

		case Qt::FontRole:
		{
			QFont font;
			if (!action->data().isNull())
				font.setItalic(true);
			return font;
		}

		case ActionRole:
			return QVariant::fromValue<QAction *>(action);
	}

	return QVariant();
}

QModelIndex ActionsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	return index(sourceIndex.row() + VisibleBeforeActions.count(), sourceIndex.column());
}

QModelIndex ActionsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	int row = proxyIndex.row();
	if (row < VisibleBeforeActions.count() || row >= VisibleBeforeActions.count() + sourceModel()->rowCount())
		return QModelIndex();
	return sourceModel()->index(row - VisibleBeforeActions.count(), proxyIndex.column());
}
