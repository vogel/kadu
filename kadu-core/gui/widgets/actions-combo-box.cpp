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

#include <QtGui/QAction>

#include "model/roles.h"

#include "actions-combo-box.h"

ActionsComboBox::ActionsComboBox(QWidget *parent) :
		QComboBox(parent),
		SourceModel(0), SourceProxyModel(0), ActionsModel(new ActionsProxyModel(this)),
		DataRole(0), LastIndex(-1)
{
	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ActionsComboBox::~ActionsComboBox()
{
}

bool ActionsComboBox::isActionSelectable(QAction *action)
{
	if (!action) // every non-action is selectable
		return true;
	if (action->data().isNull()) // by default every action is selectable
		return true;
	// check if action has 'unselectable' flag set to true
	return !action->data().toBool();
}

void ActionsComboBox::activatedSlot(int index)
{
	QModelIndex modelIndex = ActionsModel->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (!action)
		return;

	if (!isActionSelectable(action))
		setCurrentIndex(LastIndex);

	action->trigger();
}

void ActionsComboBox::currentIndexChangedSlot(int index)
{
	if (index < 0 || index >= count())
	{
		setCurrentIndex(0);
		return;
	}

	QModelIndex modelIndex = ActionsModel->index(index, modelColumn(), rootModelIndex());
	QModelIndex lastModelIndex = ActionsModel->index(LastIndex, modelColumn(), rootModelIndex());

	QAction *action = modelIndex.data(ActionRole).value<QAction *>();
	QVariant lastValue = lastModelIndex.data(DataRole);
	QVariant currentValue = ActionsModel->index(index, modelColumn()).data(DataRole);

	if (isActionSelectable(action))
		LastIndex = index;

	if (!compare(currentValue, lastValue))
		valueChanged(currentValue, lastValue);
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 *
 * Makes sure that if currently selected item was removed from the model,
 * current index is set to 0.
 *
 * Needs to be called whenever rowsRemoved() signal is emitted from the model.
 */
void ActionsComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	if (parent != rootModelIndex())
		return;

	if ((LastIndex >= start) && (LastIndex <= end))
		reset();
}

void ActionsComboBox::setDataRole(int dataRole)
{
	DataRole = dataRole;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafal 'Vogel' Malinowski
 * @param sourceModel the model to be set (must be an instance of KaduAbstractModel)
 * @param sourceProxyModel (optional) proxy model to be set
 *
 * Sets up the model in combo box. Must be called before any signals are
 * connected to/from the model.
 */
void ActionsComboBox::setUpModel(QAbstractItemModel *sourceModel, QAbstractProxyModel *sourceProxyModel)
{
	SourceModel = sourceModel;
	SourceProxyModel = sourceProxyModel;

	if (!dynamic_cast<KaduAbstractModel *>(SourceModel))
	{
		qDebug("ActionsComboBox::setUpModel(): error: sourceModel not an instace of KaduAbstractModel");
		SourceModel = 0;
		SourceProxyModel = 0;
		return;
	}

	if (SourceProxyModel)
	{
		SourceProxyModel->setSourceModel(SourceModel);
		ActionsModel->setSourceModel(SourceProxyModel);
	}
	else
		ActionsModel->setSourceModel(SourceModel);

	setModel(ActionsModel);

	connect(ActionsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafal 'Vogel' Malinowski
 * @param value value to be set
 *
 * Sets current value to the value in argument. If the value has
 * actually changed, currentIndexChangedSlot() should return true
 * and update current value (note that it has to be called when
 * currentIndexChanged() signal is emitted by a combo box).
 */
void ActionsComboBox::setCurrentValue(QVariant value)
{
	if (!SourceModel)
		return;

	QModelIndex index = dynamic_cast<KaduAbstractModel *>(SourceModel)->indexForValue(value);
	if (SourceProxyModel)
		index = SourceProxyModel->mapFromSource(index);
	index = ActionsModel->mapFromSource(index);
	setCurrentIndex(index.row());
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafal 'Vogel' Malinowski
 * @return current value
 *
 * Makes sure current value is set to the value pointed by current index
 * and returns that value.
 */
QVariant ActionsComboBox::currentValue()
{
	return ActionsModel->index(currentIndex(), modelColumn()).data(DataRole);
}

void ActionsComboBox::valueChanged(QVariant value, QVariant previousValue)
{
	Q_UNUSED(value)
	Q_UNUSED(previousValue)
}

void ActionsComboBox::addBeforeAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility)
{
	ActionsModel->addBeforeAction(action, actionVisibility);
}

void ActionsComboBox::addAfterAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility)
{
	ActionsModel->addAfterAction(action, actionVisibility);
}

QAction * ActionsComboBox::currentAction()
{
	return ActionsModel->index(currentIndex(), modelColumn()).data(ActionRole).value<QAction *>();
}

void ActionsComboBox::reset()
{
	setCurrentIndex(0);
}
