/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef MODEL_CHAIN_H
#define MODEL_CHAIN_H

#include <QtCore/QModelIndexList>
#include <QtCore/QObject>

#include "model/kadu-abstract-model.h"
#include "exports.h"

class QAbstractItemModel;
class QAbstractProxyModel;

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class ModelChain
 * @author Rafał 'Vogel' Malinowski
 * @short Chain build from one normal model and list of proxy models.
 *
 * This class bundles model and list of proxy models into one object that allows to easily
 * get index of given element in last proxy model. First model of chain must be of type QAbstractProxyModel
 * and must derive from KaduAbstractModel interface to provide indexListForValue method.
 *
 * ModelChain can be used as part of KaduMergedProxyModel.
 */
class KADUAPI ModelChain : public QObject, public KaduAbstractModel
{
	Q_OBJECT

	QAbstractItemModel *Model;
	QList<QAbstractProxyModel *> ProxyModels;

	KaduAbstractModel *KaduModel;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new chain with given base model.
	 * @param parent parent of chain
	 *
	 * This contructor creates new chain.
	 */
	explicit ModelChain(QObject *parent = 0);
	virtual ~ModelChain();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets different base model for given chain.
	 * @param model new base model
	 *
	 * Sets new default base model for chain. This model must be KaduAbstractModel.
	 */
	void setBaseModel(QAbstractItemModel *model);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new proxy model at the end of chain.
	 * @param proxyModel new proxy model
	 *
	 * This metod adds new proxy model at the end of chain.
	 */
	void addProxyModel(QAbstractProxyModel *proxyModel);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns base model of chain.
	 * @return base model of chain
	 *
	 * Returns base model of chain.
	 */
	QAbstractItemModel * firstModel() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns last chain model.
	 * @return last chain model
	 *
	 * Returns last chain model. When chain does not have any proxy model result of this method is identical
	 * to firstModel. In other case last added proxy model is returned.
	 */
	QAbstractItemModel * lastModel() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of indexes of given value in last proxy model.
	 * @return index of given value in last proxy model
	 *
	 * Returns list of indexes of given value in last proxy model. This method uses KaduAbstractModel::indexListForValue
	 * on base model to get first list of indexes. Then it uses mapFromSource on each proxy model to finally
	 * get indexes on last model in chain.
	 */
	QModelIndexList indexListForValue(const QVariant &value) const;

};

/**
 * @}
 */

#endif // MODEL_CHAIN_H
