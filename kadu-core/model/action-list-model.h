/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTION_LIST_MODEL_H
#define ACTION_LIST_MODEL_H

#include <QtCore/QAbstractItemModel>

#include "model/kadu-abstract-model.h"

class QAction;
class QModelIndex;

class ActionListModel : public QAbstractItemModel, public KaduAbstractModel
{
	Q_OBJECT

	QList<QAction *> ActionList;

public:
	explicit ActionListModel(QObject *parent = 0);
	virtual ~ActionListModel();

	void setActionList(const QList<QAction *> &actionList);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags (const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	// KaduAbstractModel implementation
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

#endif // ACTION_LIST_MODEL_H
