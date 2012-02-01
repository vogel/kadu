/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

class ActionFilterProxyModel;
class ActionListModel;
class KaduAbstractModel;
class ModelChain;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ActionsComboBox
 * @author Bartosz 'beevvy' Brachaczek
 * @author Rafał 'Vogel' Malinowski
 * @short QComboBox with support for actions and QVariant data.
 *
 * This class is an extension to QComboBox that uses MergedProxyModel to add two models with actions before and
 * after main model of the combo box.
 *
 * This allows this widget  to have actions like "select", "create new" or "edit". Actions can be marked
 * as unselectable. Unselectable action can only be triggered, selection is immediately restored to the previous
 * value item or selectable action. Two methods  are available for adding actions: addBeforeAction() (to add action
 * at the begining of list) and addAfterAction() (to add action at the end of list). Method currentAction() is
 * available for getting currently selected action.
 *
 * Data is stored in model that derives from QAbstractItemModel and KaduAbstractModel classes. It is possible to
 * use proxy model derived from QAbstractProxyModel. Data is stored as QVariant object with role passed in
 * protected setUpModel method.
 */
class ActionsComboBox : public QComboBox
{
	Q_OBJECT

public:
	enum ActionVisibility // flag
	{
		AlwaysVisible,
		// TODO: think of better names
		NotVisibleWithEmptySourceModel,
		NotVisibleWithOneRowSourceModel
	};

private:
	ActionListModel *BeforeActions;
	ActionListModel *AfterActions;
	ActionFilterProxyModel *ActionsFilterModel;
	KaduAbstractModel *KaduModel;

	int DataRole;
	int LastIndex;

	bool isActionSelectable(QAction *action);
	void addActionToFilter(QAction *action, ActionVisibility visibility);

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
	 * This slot is called when current selected is changed by user or from code. If new index does not contain unselectable
	 * action then this index is stored as safe selectable index.
	 */
	void currentIndexChangedSlot(int index);

protected:
	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets up model for combo box.
	 * @param dataRole role used to get data from model
	 * @param modelChain chain of models to use as data source
	 *
	 * Sets up the model in combo box. Must be called before any signals are connected to/from the model.
	 * Parameter modelChain is modified in this method.
	 */
	void setUpModel(int dataRole, ModelChain *modelChain);

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
	QVariant currentValue() const;

public:
	explicit ActionsComboBox(QWidget *parent = 0);
	virtual ~ActionsComboBox();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add new action before real values in combo box.
	 * @param action action to add
	 * @param visibility action visibility of new action
	 *
	 * Add new action before real values in combo box. Action will be triggered when selected by user.
	 * Action can be marked as non-selectable by setting its data property to true. Second parameter
	 * creates connection between size of data model and this action visibility.
	 */
	void addBeforeAction(QAction *action, ActionVisibility visibility = AlwaysVisible);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Add new action after real values in combo box.
	 * @param action action to add
	 * @param visibility action visibility of new action
	 *
	 * Add new action after real values in combo box. Action will be triggered when selected by user.
	 * Action can be marked as non-selectable by setting its data property to true. Second parameter
	 * creates connection between size of data model and this action visibility.
	 */
	void addAfterAction(QAction *action, ActionVisibility visibility = AlwaysVisible);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns currently selected action.
	 * @return currently selected action
	 *
	 * Returns currently selected action.
	 */
	QAction * currentAction();

};

/**
 * @}
 */

#endif // ACTIONS_COMBO_BOX_H
