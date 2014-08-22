/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIGURABLE_TRANSFORMER_SERVICE_H
#define CONFIGURABLE_TRANSFORMER_SERVICE_H

#include <QtCore/QList>

/**
 * @addtogroup Services
 * @{
 */

/**
 * @class ConfigurableTransformerService
 * @short Template for services that registers list of ConfigurableTransformer instances.
 * @author Rafał 'Vogel' Malinowski
 */
template<typename T>
class ConfigurableTransformerService
{
	QList<T *> ConfigurableTransformers;

public:
	void registerTransformer(T *transformer)
	{
		if (!transformer || ConfigurableTransformers.contains(transformer))
			return;

		ConfigurableTransformers.append(transformer);
	}

	void unregisterTransformer(T *transformer)
	{
		ConfigurableTransformers.removeAll(transformer);
	}

	typename T::object_type transform(const typename T::object_type &object, const typename T::config_type &configurationObject)
	{
		typename T::object_type result = object;
		foreach (T *transformer, ConfigurableTransformers)
			result = transformer->transform(result, configurationObject);
		return result;
	}

};

/**
 * @}
 */

#endif // CONFIGURABLE_TRANSFORMER_SERVICE_H
