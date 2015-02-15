/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTION_FILTER_PROXY_MODEL_H
#define ACTION_FILTER_PROXY_MODEL_H

#include <QtCore/QSortFilterProxyModel>

class QAction;

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class ActionFilterProxyModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model that allows for filtering out action based on size of given model.
 *
 * This model can be used to hide actions when some other model is empty or have only one item.
 * This class can be used on merged models that consists of some action models and one main model.
 * Some actions should not be visible if main model contains only one item or does not contain any.
 * "Select item" action or separator actions are best examples.
 *
 * Please note that both QSortFilterProxyModel::setSourceModel() and setModel() must be called (with
 * different models!) to make best use of this class. For example:
 *
 * QAbstractItemModel *mainModel;
 * QAbstractItemModel *mergedModel; // = beforeActionsModel + mainModel + afterActionsModel
 * ActionFilterProxyModel *proxyModel;
 *
 * proxyModel->setSourceModel(mergedModel); // model to filter
 * proxyModel->setModel(mainModel); // model to base filtering on
 */
class ActionFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	int Size;
	QAbstractItemModel *Model;
	QList<QAction *> HideWhenModelEmpty;
	QList<QAction *> HideWhenModelSingle;

private slots:
	void updateSize();

protected:
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of ActionFilterProxyModel
	 * @param parent QObject parent of new model
	 */
	explicit ActionFilterProxyModel(QObject *parent = 0);
	virtual ~ActionFilterProxyModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets data model for filtering.
	 * @param model data model for filtering
	 *
	 * This new model will be used as data source for filtering. When this model is empty, all actions
	 * added by addHideWhenModelEmpty() and addHideWhenModelSingle() will be filered out from sourceModel().
	 * When this model contains only one item, all actions added by addHideWhenModelSingle() will be
	 * filered out from sourceModel().
	 */
	void setModel(QAbstractItemModel *model);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds new action to filter out when main model is empty.
	 * @param action new action to filter out when main model is empty
	 */
	void addHideWhenModelEmpty(QAction *action);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Adds new action to filter out when main model is empty or contains only one item.
	 * @param action new action to filter out when main model is empty or contains only one item
	 */
	void addHideWhenModelSingle(QAction *action);

};

/**
 * @}
 */

#endif // ACTION_FILTER_PROXY_MODEL_H
