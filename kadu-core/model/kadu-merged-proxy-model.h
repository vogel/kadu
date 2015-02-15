/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_MERGED_PROXY_MODEL_H
#define KADU_MERGED_PROXY_MODEL_H

#include "model/kadu-abstract-model.h"
#include "model/merged-proxy-model.h"

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class KaduMergedProxyModel
 * @author Rafał 'Vogel' Malinowski
 * @short Model that merges other models into one and allows for calling indexListForValue method.
 *
 * This model merges other models into one. KaduMergedProxyModel accepts all ModelChain  objects and
 * QAbstractItemModel that implements KaduAbstractModel interface and.
 *
 * It is possible to retreive original indexes of any item using indexListForValue method.
 */
class KaduMergedProxyModel : public MergedProxyModel, public KaduAbstractModel
{
	Q_OBJECT

	QList<KaduAbstractModel *> KaduModels;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of KaduMergedProxyModel.
	 * @param parent QObject parent of new KaduMergedProxyModel.
	 *
	 * Creates new instance of KaduMergedProxyModel.
	 */
	explicit KaduMergedProxyModel(QObject *parent = 0);
	virtual ~KaduMergedProxyModel();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of models to merge.
	 * @param models list of models to merge
	 *
	 * Sets list of models to merge. All models must implement QAbstractItemModel interface or be
	 * ModelChain interface, otherwise an assertion is thrown.
	 */
	void setKaduModels(QList<KaduAbstractModel *> models);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return list of indexes that holds given object.
	 * @return list of indexes that holds given object
	 * @param value object to find
	 *
	 * This method returns all indexes that holds given object. It does that by searching all
	 * submodels and mapping its' indexes to own indexes.
	 */
	virtual QModelIndexList indexListForValue(const QVariant &value) const;

};

/**
 * @}
 */

#endif // KADU_MERGED_PROXY_MODEL_H
