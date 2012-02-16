/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MERGED_PROXY_MODEL_FACTORY_H
#define MERGED_PROXY_MODEL_FACTORY_H

#include "exports.h"

class QAbstractItemModel;

class KaduAbstractModel;

/**
 * @addtogroup Model
 * @{
 */

/**
 * @class MergedProxyModelFactory
 * @author Rafał 'Vogel' Malinowski
 * @short Factory that provides instances of model that merges other models into one.
 *
 * Factory that provides instances of model that merges other models into one.
 */
namespace MergedProxyModelFactory
{
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of model that merges other models into one.
	 * @param models list of models to merge
	 * @param parent qobject parent of new model
	 * @return new instance of model that merges other models into one
	 */
	KADUAPI QAbstractItemModel * createInstance(QList<QAbstractItemModel *> models, QObject *parent = 0);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new instance of model that merges other models into one.
	 * @param models list of models to merge
	 * @param parent qobject parent of new model
	 * @return new instance of model that merges other models into one
	 *
	 * The only difference between this method and createInstance is that every model in list must implement
	 * KaduAbstractModel interface and resulting merged model will also implement this interface.
	 *
	 * Accepted items must either be ModelChain or QAbstractItemModel objects.
	 */
	KADUAPI QAbstractItemModel * createKaduModelInstance(QList<KaduAbstractModel *> models, QObject *parent = 0);

}

/**
 * @}
 */

#endif // MERGED_PROXY_MODEL_FACTORY_H
