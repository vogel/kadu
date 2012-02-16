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

#include "exports.h"
#include "model/kadu-abstract-model.h"

class QAction;
class QModelIndex;

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class ActionListModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model that contains list of QAction * items.
 *
 * This model can contain list of QAction * items. It implements KaduAbstractModel so it is possible
 * to get back index of given QAction *.
 */
class KADUAPI ActionListModel : public QAbstractItemModel, public KaduAbstractModel
{
	Q_OBJECT

	QList<QAction *> ActionList;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of object with given parent.
	 * @param parent QObject parent of new model
	 */
	explicit ActionListModel(QObject *parent = 0);
	virtual ~ActionListModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of actions to this model.
	 * @param actionList list of actions for this model
	 */
	void setActionList(const QList<QAction *> &actionList);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Appends new action at the end of list.
	 * @param action action to append at the end of list
	 */
	void appendAction(QAction *action);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Inserts new action at given position.
	 * @param index position of new action
	 * @param action action to insert at given position
	 *
	 * If index is less or equal to zero, new action is prepended to list. If index is
	 * equal of greater than size of list it is appended to it. In other case it is inserted
	 * at given index between existing actions.
	 */
	void insertAction(int index, QAction *action);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Removes given action.
	 * @param action action to remove
	 *
	 * This method only removes one instance of action from list.
	 */
	void removeAction(QAction *action);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags (const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	// KaduAbstractModel implementation
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

/**
 * @}
 */

#endif // ACTION_LIST_MODEL_H
