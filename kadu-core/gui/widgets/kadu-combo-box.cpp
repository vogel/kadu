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

#include "kadu-combo-box.h"

KaduComboBox::KaduComboBox(QWidget *parent) :
		QComboBox(parent), DataRole(0),
		SourceModel(0), SourceProxyModel(0), ActionsModel(new ActionsProxyModel(this))
{
}

KaduComboBox::~KaduComboBox()
{
}

void KaduComboBox::setDataRole(int dataRole)
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
void KaduComboBox::setUpModel(QAbstractItemModel *sourceModel, QAbstractProxyModel *sourceProxyModel)
{
	SourceModel = sourceModel;
	SourceProxyModel = sourceProxyModel;

	if (!dynamic_cast<KaduAbstractModel *>(SourceModel))
	{
		qDebug("KaduComboBox::setUpModel(): error: sourceModel not an instace of KaduAbstractModel");
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
	connect(ActionsModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
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
void KaduComboBox::setCurrentValue(QVariant value)
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
QVariant KaduComboBox::currentValue()
{
	if (SourceModel)
		CurrentValue = ActionsModel->index(currentIndex(), modelColumn()).data(DataRole);
	return CurrentValue;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafal 'Vogel' Malinowski
 * @return whether current value changed
 *
 * Makes sure that correct current value is set after the index has changed
 * and returns true in case current value actually has changed, otherwise
 * false.
 */
bool KaduComboBox::currentIndexChangedSlot(int index)
{
	if (index < 0 || (index >= count() && count() != 0))
		setCurrentIndex(0);
	else
	{
		updateValueBeforeChange(); // sets ValueBeforeChange variable
		currentValue(); // sets CurrentValue variable

		if (!compare(CurrentValue, ValueBeforeChange))
			return true;
	}

	return false;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 *
 * Updates ValueBeforeChange field.
 *
 * Needs to be called whenever rowsToBeRemoved() signal is emitted from
 * the model.
 */
void KaduComboBox::updateValueBeforeChange()
{
	ValueBeforeChange = CurrentValue;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 *
 * Makes sure that if currently selected item was removed from the model,
 * current index is set to 0.
 *
 * Needs to be called whenever rowsRemoved() signal is emitted from the model.
 */
void KaduComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	if (parent != rootModelIndex())
		return;

	if (!compare(CurrentValue, ValueBeforeChange))
		setCurrentIndex(0);
}

QVariant KaduComboBox::data(int role) const
{
	return ActionsModel->index(currentIndex(), modelColumn()).data(role);
}
