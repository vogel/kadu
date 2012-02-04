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

#include "model/action-filter-proxy-model.h"
#include "model/action-list-model.h"
#include "model/roles.h"
#include "model/merged-proxy-model-factory.h"
#include "model/model-chain.h"

#include "actions-combo-box.h"

Q_DECLARE_METATYPE(QAction *);

ActionsComboBox::ActionsComboBox(QWidget *parent) :
		QComboBox(parent),
		KaduModel(0), DataRole(0), LastIndex(-1)
{
	BeforeActions = new ActionListModel(this);
	AfterActions = new ActionListModel(this);
	ActionsFilterModel = new ActionFilterProxyModel(this);

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
	QModelIndex modelIndex = model()->index(index, modelColumn(), rootModelIndex());
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

	QModelIndex modelIndex = model()->index(index, modelColumn(), rootModelIndex());
	QModelIndex lastModelIndex = model()->index(LastIndex, modelColumn(), rootModelIndex());

	QAction *action = modelIndex.data(ActionRole).value<QAction *>();
	QVariant lastValue = lastModelIndex.data(DataRole);
	QVariant currentValue = model()->index(index, modelColumn()).data(DataRole);

	if (isActionSelectable(action))
		LastIndex = index;
}

void ActionsComboBox::setUpModel(int dataRole, ModelChain *modelChain)
{
	DataRole = dataRole;

	QList<KaduAbstractModel *> models;
	models.append(BeforeActions);
	models.append(modelChain);
	models.append(AfterActions);

	QAbstractItemModel *mergedModel = MergedProxyModelFactory::createKaduModelInstance(models, this);
	Q_ASSERT(mergedModel);

	KaduModel = dynamic_cast<KaduAbstractModel *>(mergedModel);
	Q_ASSERT(KaduModel);

	ActionsFilterModel->setSourceModel(mergedModel);
	ActionsFilterModel->setModel(modelChain->lastModel());

	setModel(ActionsFilterModel);
}

void ActionsComboBox::setCurrentValue(const QVariant &value)
{
	if (!KaduModel)
		return;

	const QModelIndexList &indexes = KaduModel->indexListForValue(value);
	if (indexes.isEmpty())
	{
		setCurrentIndex(0);
		return;
	}

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	setCurrentIndex(index.row());
}

QVariant ActionsComboBox::currentValue() const
{
	return model()->index(currentIndex(), modelColumn()).data(DataRole);
}

void ActionsComboBox::addActionToFilter(QAction *action, ActionsComboBox::ActionVisibility visibility)
{
	switch (visibility)
	{
		case NotVisibleWithEmptySourceModel:
			ActionsFilterModel->addHideWhenModelEmpty(action);
			break;
		case NotVisibleWithOneRowSourceModel:
			ActionsFilterModel->addHideWhenModelSingle(action);
			break;
		default:
			break;
	}
}

void ActionsComboBox::addBeforeAction(QAction *action, ActionVisibility visibility)
{
	BeforeActions->appendAction(action);
	addActionToFilter(action, visibility);
}

void ActionsComboBox::addAfterAction(QAction *action, ActionVisibility visibility)
{
	AfterActions->appendAction(action);
	addActionToFilter(action, visibility);
}

QAction * ActionsComboBox::currentAction()
{
	return model()->index(currentIndex(), modelColumn()).data(ActionRole).value<QAction *>();
}
