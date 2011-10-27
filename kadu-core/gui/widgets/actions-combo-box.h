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

#include <QtGui/QComboBox>

#include "model/actions-proxy-model.h"

#include "model/kadu-abstract-model.h"

class QAbstractProxyModel;

/**
 * @addtogroup Gui
 * @{
 */
/**
 * @class ActionsComboBox
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafał 'Vogel' Malinowski
 * @short QComboBox with support for actions and QVariant data.
 * @see ActionsProxyModel
 *
 * This class is an extension to QComboBox that uses ActionsProxyModel as data source. This allows this widget
 * to have actions like "select", "create new" or "edit. Actions can be marked as unselectable. Unselectable action
 * can only be triggerred, selection is immediately restored to previous value item or selectable action. Two methods
 * are available to add actions: addBeforeAction (to add action at the begining of list) and addAfterAction (to add
 * action at the end of list). Method currentAction() is available for getting currently selected action.
 *
 * Data is stored in model that derives from QAbstractItemModel and KaduAbstractModel classes. It is possible to
 * use proxy model derivered from QAbstractProxyModel. Data is stored as QVariant object with role passed in
 * protected setUpModel method. As QVariants cannot be compared directly, compare(QVariant, QVariant) abstract
 * method must be reimplemented in each derivered class.
 */
class ActionsComboBox : public QComboBox
{
	Q_OBJECT

	QAbstractItemModel *SourceModel;
	QAbstractProxyModel *SourceProxyModel;
	ActionsProxyModel *ActionsModel;

	int DataRole;
	int LastIndex;

	bool isActionSelectable(QAction *action);

private slots:
	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after user changed selected item.
	 * @param index activated index
	 *
	 * This slot is called when user selected new value in combo box. If new index contains unselectable action
	 * previous selection is restored and action is triggered. If new index contains selectable action, then this
	 * action is triggered.
	 */
	void activatedSlot(int index);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after selected index has changed.
	 * @param index new selected index index
	 *
	 * This slot is called when current selected is changed by user or from code. If new index doe not contains unselectable
	 * action then this index is stored as safe selectable index. If this change in selection changed current selected
	 * value (checked by compare(QVariant, QVariant) method) then valueChanged(QVariant, QVariant) abstract method is called.
	 */
	void currentIndexChangedSlot(int index);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after some rows were removed from model.
	 * @param parent parent index of removed rows
	 * @param start first removed row under parent
	 * @param end last removed row under parent
	 *
	 * This slot is called after some rows were removed from model. If current index was in these rows
	 * reset() virtual method is called to restore this widget to valid state.
	 */
	void rowsRemoved(const QModelIndex &parent, int start, int end);

protected:
	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets up model for combo box.
  * @param dataRole role used to get data from model
	 * @param sourceModel the model to be set (must be an instance of KaduAbstractModel)
	 * @param sourceProxyModel (optional) proxy model to be set
	 *
	 * Sets up the model in combo box. Must be called before any signals are connected to/from the model.
	 */
	void setUpModel(int dataRole, QAbstractItemModel *sourceModel, QAbstractProxyModel *sourceProxyModel = 0);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets current value to the value in argument.
	 * @param value value to be set
	 *
	 * Sets current value to the value in argument.
	 */
	void setCurrentValue(const QVariant &value);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns current value.
	 * @return current value
	 *
	 * Returns current value.
	 */
	QVariant currentValue();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short This method is called every time selected value has changed.
	 * @param value new selected value
	 * @param previousValue previous selected value
	 *
	 * This method is called every time selected value has changed.
	 */
	virtual void valueChanged(const QVariant &value, const QVariant &previousValue);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Compares two QVariant objects.
	 * @param left value to compare
	 * @param right value to compare
	 * @return true if both values are equal
	 *
	 * As QVariants aren't comparable this method must be reimplemted in every derived class to allow comparison
	 * of QVariants. This method return true when both values are equal.
	 */
	virtual bool compare(const QVariant &left, const QVariant &right) const = 0;

public:
	explicit ActionsComboBox(QWidget *parent = 0);
	virtual ~ActionsComboBox();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add new action before real values in combo box.
	 * @param action action to add
	 * @param actionVisibility action visibility of new action
	 *
	 * Add new action before real values in combo box. Action will be triggered when selected by user.
	 * Action can be marked as non-selectable by setting its data property to true. Second parameter
	 * creates connection between size of data model and this action visibility.
	 */
	void addBeforeAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility = ActionsProxyModel::AlwaysVisible);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add new action after real values in combo box.
	 * @param action action to add
	 * @param actionVisibility action visibility of new action
	 *
	 * Add new action after real values in combo box. Action will be triggered when selected by user.
	 * Action can be marked as non-selectable by setting its data property to true. Second parameter
	 * creates connection between size of data model and this action visibility.
	 */
	void addAfterAction(QAction *action, ActionsProxyModel::ActionVisibility actionVisibility = ActionsProxyModel::AlwaysVisible);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns currently selected action.
	 * @return currently selected action
	 *
	 * Returns currently selected action.
	 */
	QAction * currentAction();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Resets this combo box selection.
	 *
	 * Resets this combo box selection after currently selected item was removed from model. Default implementations
	 * sets selection index to 0.
	 */
	virtual void resetSelection();

};

/**
 * @}
 */

#endif // ACTIONS_COMBO_BOX_H
