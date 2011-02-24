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

#ifndef KADU_COMBO_BOX_H
#define KADU_COMBO_BOX_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QAbstractProxyModel>
#include <QtGui/QAction>
#include <QtGui/QComboBox>

#include "model/actions-proxy-model.h"
#include "model/kadu-abstract-model.h"

template<class T>
class KaduComboBox : public QComboBox
{
protected:
	T CurrentValue;
	T ValueBeforeChange;

	QAbstractItemModel *SourceModel;
	QAbstractProxyModel *SourceProxyModel;
	ActionsProxyModel *ActionsModel;

	virtual int preferredDataRole() const = 0;
	virtual QString selectString() const = 0;
	virtual ActionsProxyModel::ActionVisibility selectVisibility() const
	{
		return ActionsProxyModel::AlwaysVisible;
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
	void setUpModel(QAbstractItemModel *sourceModel, QAbstractProxyModel *sourceProxyModel = 0)
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

		if (!selectString().isEmpty())
			ActionsModel->addBeforeAction(new QAction(selectString(), this), selectVisibility());

		if (SourceProxyModel)
		{
			SourceProxyModel->setSourceModel(SourceModel);
			ActionsModel->setSourceModel(SourceProxyModel);
		}
		else
			ActionsModel->setSourceModel(SourceModel);

		setModel(ActionsModel);
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
	void setCurrentValue(T value)
	{
		if (!SourceModel)
			return;

		QModelIndex index =
			dynamic_cast<KaduAbstractModel *>(SourceModel)->indexForValue(QVariant::fromValue<T>(value));
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
	T currentValue()
	{
		if (SourceModel)
		{
			// g++ doesn't accept that syntax for some reason
			//CurrentValue = ActionsModel->index(currentIndex(), modelColumn()).data(preferredDataRole()).value<T>();
			CurrentValue = qVariantValue<T>(ActionsModel->index(currentIndex(), modelColumn()).data(preferredDataRole()));
		}
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
	bool currentIndexChangedSlot(int index)
	{
		if (index < 0 || (index >= count() && count() != 0))
			setCurrentIndex(0);
		else
		{
			updateValueBeforeChange(); // sets ValueBeforeChange variable
			currentValue(); // sets CurrentValue variable

			if (CurrentValue != ValueBeforeChange)
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
	void updateValueBeforeChange()
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
	void rowsRemoved(const QModelIndex &parent, int start, int end)
	{
		Q_UNUSED(start)
		Q_UNUSED(end)

		if (parent != rootModelIndex())
			return;

		if (CurrentValue != ValueBeforeChange)
			setCurrentIndex(0);
	}

public:
	explicit KaduComboBox(QWidget *parent = 0) :
		QComboBox(parent),
		CurrentValue(), ValueBeforeChange(),
		SourceModel(0), SourceProxyModel(0), ActionsModel(new ActionsProxyModel(this))
	{
	}

	virtual ~KaduComboBox()
	{
	}

	QVariant data(int role) const
	{
		return ActionsModel->index(currentIndex(), modelColumn()).data(role);
	}

	ActionsProxyModel * actionsModel() const { return ActionsModel; }

};

#endif // KADU_COMBO_BOX_H
