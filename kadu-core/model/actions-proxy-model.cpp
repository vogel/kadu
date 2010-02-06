/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

void ActionsProxyModel::addBeforeAction(QAction *action)
{
	beginInsertRows(QModelIndex(), BeforeActions.size(), BeforeActions.size());
	BeforeActions.append(action);
	endInsertRows();
}

void ActionsProxyModel::addAfterAction(QAction *action)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	AfterActions.append(action);
	endInsertRows();
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
		disconnect(currentModel, SIGNAL(modelAboutToBeReset()), this, SLOT(sourceAboutToBeReset()));
		disconnect(currentModel, SIGNAL(modelReset()), this, SLOT(sourceReset()));
	}

	QAbstractProxyModel::setSourceModel(newSourceModel);

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
		start += BeforeActions.size();
		end += BeforeActions.size();
	}

	beginInsertRows(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceRowsInserted(const QModelIndex &sourceParent, int start, int end)
{
	if (!sourceParent.isValid())
	{
		start += BeforeActions.size();
		end += BeforeActions.size();
	}

	endInsertRows();
}

void ActionsProxyModel::sourceColumnsAboutToBeInserted(const QModelIndex &sourceParent, int start, int end)
{
	beginInsertColumns(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceColumnsInserted(const QModelIndex &sourceParent, int start, int end)
{
	endInsertColumns();
}

void ActionsProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &sourceParent, int start, int end)
{
	if (!sourceParent.isValid())
	{
		start += BeforeActions.size();
		end += BeforeActions.size();
	}
	
	beginRemoveRows(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceRowsRemoved(const QModelIndex &sourceParent, int start, int end)
{
	endRemoveRows();
}

void ActionsProxyModel::sourceColumnsAboutToBeRemoved(const QModelIndex &sourceParent, int start, int end)
{
	beginRemoveColumns(mapFromSource(sourceParent), start, end);
}

void ActionsProxyModel::sourceColumnsRemoved(const QModelIndex &sourceParent, int start, int end)
{
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
	return sourceModel()->columnCount(mapToSource(parent));
}

int ActionsProxyModel::rowCount(const QModelIndex &parent) const
{
	if (sourceModel())
		return sourceModel()->rowCount(mapToSource(parent)) + BeforeActions.count() + AfterActions.count();
	else
		return BeforeActions.count() + AfterActions.count();
}

QModelIndex ActionsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, 0);
}

QModelIndex ActionsProxyModel::parent(const QModelIndex &child) const
{
	return QModelIndex(); // no parent, only plain list
}

Qt::ItemFlags ActionsProxyModel::flags(const QModelIndex &index) const
{
	QModelIndex sourceIndex = mapToSource(index);
	if (QModelIndex() != sourceIndex)
		return sourceModel()->flags(sourceIndex);

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ActionsProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
	QAction *action;

	int beforeIndex = proxyIndex.row();
	int afterIndex = proxyIndex.row() - BeforeActions.count() - sourceModel()->rowCount(QModelIndex());

	if (beforeIndex >= 0 && beforeIndex < BeforeActions.count())
		action = BeforeActions[proxyIndex.row()];
	else if (afterIndex >= 0 && afterIndex < AfterActions.count())
		action = AfterActions[afterIndex];
	else
		return sourceModel()->data(mapToSource(proxyIndex), role);

	switch (role)
	{
		case Qt::DisplayRole:
			return action->text();

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
	return index(sourceIndex.row() + BeforeActions.count(), sourceIndex.column());
}

QModelIndex ActionsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	int row = proxyIndex.row();
	if (row < BeforeActions.count() || row >= BeforeActions.count() + sourceModel()->rowCount(QModelIndex()))
		return QModelIndex();
	return sourceModel()->index(row - BeforeActions.count(), proxyIndex.column(), QModelIndex());
}
