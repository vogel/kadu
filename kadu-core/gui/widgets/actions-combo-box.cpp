/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/model-chain.h"
#include "model/roles.h"

#include "actions-combo-box.h"

ActionsComboBox::ActionsComboBox(QWidget *parent) :
		QComboBox(parent),
		Chain(0), ActionsModel(new ActionsProxyModel(this)),
		DataRole(0), LastIndex(-1)
{
	// Queued connection is needed here so that we do not depend on which signal is emitted first:
	// activated() or currentIndexChanged() (we depend on correct LastIndex in activatedSlot()).
	connect(this, SIGNAL(activated(int)), this, SLOT(activatedSlot(int)), Qt::QueuedConnection);
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
}

void ActionsComboBox::setUpModel(int dataRole, ModelChain *modelChain)
{
	DataRole = dataRole;
	Chain = modelChain;

	Chain->addProxyModel(ActionsModel);
	setModel(ActionsModel);
}

void ActionsComboBox::setCurrentValue(const QVariant &value)
{
	const QModelIndexList &indexes = Chain->indexListForValue(value);
	if (indexes.isEmpty())
	{
		setCurrentIndex(0);
		return;
	}

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	setCurrentIndex(index.row());
}

QVariant ActionsComboBox::currentValue()
{
	return ActionsModel->index(currentIndex(), modelColumn()).data(DataRole);
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
