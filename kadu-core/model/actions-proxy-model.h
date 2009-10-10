 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIONS_PROXY_MODEL_H
#define ACTIONS_PROXY_MODEL_H

#include <QtGui/QAbstractProxyModel>

class ActionsProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	typedef QPair<QString, QString> ModelAction;
	typedef QList<ModelAction> ModelActionList;

private:
	ModelActionList BeforeActions;
	ModelActionList AfterActions;

public:
	ActionsProxyModel(ModelActionList beforeActions, ModelActionList afterActions, QObject *parent);
	virtual ~ActionsProxyModel();

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

};

#endif // ACTIONS_PROXY_MODEL_H
