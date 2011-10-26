/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTIONS_COMBO_BOX_H
#define ACTIONS_COMBO_BOX_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QAbstractProxyModel>
#include <QtGui/QAction>
#include <QtGui/QComboBox>

#include "model/actions-proxy-model.h"
#include "model/kadu-abstract-model.h"

class ActionsComboBox : public QComboBox
{
	Q_OBJECT

	int DataRole;

	QAbstractItemModel *SourceModel;
	QAbstractProxyModel *SourceProxyModel;
	ActionsProxyModel *ActionsModel;

private slots:
	void rowsRemoved(const QModelIndex &parent, int start, int end);
	void updateValueBeforeChange();

protected:
	QVariant CurrentValue;
	QVariant ValueBeforeChange;

	void setDataRole(int dataRole);

	virtual bool compare(QVariant value, QVariant previousValue) const = 0;

	void setUpModel(QAbstractItemModel *sourceModel, QAbstractProxyModel *sourceProxyModel = 0);

	void setCurrentValue(QVariant value);
	QVariant currentValue();

	bool currentIndexChangedSlot(int index);

public:
	explicit ActionsComboBox(QWidget *parent = 0);
	virtual ~ActionsComboBox();

	void addBeforeAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility = ActionsProxyModel::AlwaysVisible);
	void addAfterAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility = ActionsProxyModel::AlwaysVisible);

	QAction * currentAction();

};

#endif // ACTIONS_COMBO_BOX_H
