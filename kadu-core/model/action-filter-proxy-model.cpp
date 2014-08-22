/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "action-filter-proxy-model.h"

ActionFilterProxyModel::ActionFilterProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent), Size(0), Model(0)
{
}

ActionFilterProxyModel::~ActionFilterProxyModel()
{
}

void ActionFilterProxyModel::setModel(QAbstractItemModel *model)
{
	if (Model)
		disconnect(Model, 0, this, 0);

	Model = model;

	if (Model)
	{
		connect(Model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateSize()));
		connect(Model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateSize()));
	}

	updateSize();
}

void ActionFilterProxyModel::updateSize()
{
	int newSize = Model ? Model->rowCount() : 0;
	if (newSize == Size)
		return;

	Size = newSize;
	invalidateFilter();
}

void ActionFilterProxyModel::addHideWhenModelEmpty(QAction *action)
{
	HideWhenModelEmpty.append(action);
	invalidateFilter();
}

void ActionFilterProxyModel::addHideWhenModelSingle(QAction *action)
{
	HideWhenModelSingle.append(action);
	invalidateFilter();
}

bool ActionFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (Size > 1)
		return true;

	// only filter first-level
	if (sourceParent.isValid())
		return true;

	const QModelIndex &sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
	if (ActionRole != sourceIndex.data(ItemTypeRole).value<int>())
		return true;

	QAction *action = sourceIndex.data(ActionRole).value<QAction *>();
	if (!action)
		return false;

	if (Size <= 1 && HideWhenModelSingle.contains(action))
		return false;
	if (Size == 0 && HideWhenModelEmpty.contains(action))
		return false;

	return true;
}

#include "moc_action-filter-proxy-model.cpp"
