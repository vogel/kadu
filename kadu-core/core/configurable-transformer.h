/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CONFIGURABLE_TRANSFORMER_H
#define CONFIGURABLE_TRANSFORMER_H

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class ConfigurableTransformer
 * @short Interface for transforming instances of one type.
 * @author Rafał 'Vogel' Malinowski
 * @param T type to transform
 * @param T_config type of configuration object
 *
 * Implementations of this interface are used to transform object - like updating data or removing some items from list.
 */
template <typename T, typename T_config>
class ConfigurableTransformer
{

public:
	typedef T object_type;
	typedef T_config config_type;

	virtual ~ConfigurableTransformer<T, T_config>() {}

	/**
	 * @short Transform instance of class.
	 * @author Rafał 'Vogel' Malinowski
	 * @param object object to transform
	 * @param configurationObject object use to configure transformation parameters
	 * @return transformed object
	 *
	 * This method returns new object. Original one is untouched.
	 */
	virtual T transform(const T &object, const T_config &configurationObject) = 0;

};

/**
 * @}
 */

#endif // CONFIGURABLE_TRANSFORMER_H
