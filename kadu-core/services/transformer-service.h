/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TRANSFORMER_SERVICE_H
#define TRANSFORMER_SERVICE_H

#include <QtCore/QList>

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class TransformerService
 * @short Template for services that registers list of Transformer instances.
 * @author Rafał 'Vogel' Malinowski
 */
template<typename T>
class TransformerService
{
	QList<T *> Transformers;

public:
	void registerTransformer(T *transformer)
	{
		if (!transformer || Transformers.contains(transformer))
			return;

		Transformers.append(transformer);
	}

	void unregisterTransformer(T *transformer)
	{
		Transformers.removeAll(transformer);
	}

	typename T::object_type transform(const typename T::object_type &object)
	{
		typename T::object_type result = object;
		foreach (T *transformer, Transformers)
			result = transformer->transform(result);
		return result;
	}

};

/**
 * @}
 */

#endif // TRANSFORMER_SERVICE_H
