 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QFont>

#include "actions-proxy-model.h"

ActionsProxyModel::ActionsProxyModel(ModelActionList beforeActions, ModelActionList afterActions, QObject *parent) :
		QAbstractProxyModel(parent), BeforeActions(beforeActions), AfterActions(afterActions)
{
}

ActionsProxyModel::~ActionsProxyModel()
{
}

int ActionsProxyModel::columnCount(const QModelIndex &parent) const
{
	return sourceModel()->columnCount(parent);
}

int ActionsProxyModel::rowCount(const QModelIndex &parent) const
{
	return sourceModel()->rowCount(parent) + BeforeActions.count() + AfterActions.count();
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
	ModelAction action;

	if (proxyIndex.row() < BeforeActions.count())
		action = BeforeActions[proxyIndex.row()];
	else if (proxyIndex.row() >= BeforeActions.count() + sourceModel()->rowCount(QModelIndex()))
		action = AfterActions[proxyIndex.row() - BeforeActions.count()];
	else
		return sourceModel()->data(mapToSource(proxyIndex), role);

	switch (role)
	{
		case Qt::DisplayRole:
			return action.first;

		case Qt::FontRole:
			QFont font;
			font.setItalic(true);
			return font;
	}
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
