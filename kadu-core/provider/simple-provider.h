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

#ifndef SIMPLE_PROVIDER_H
#define SIMPLE_PROVIDER_H

#include "provider/provider.h"

/**
 * @addtogroup Provider
 * @{
 */

/**
 * @class SimpleProvider
 * @author Rafał 'Vogel' Malinowski
 * @short Simple implementation of Provider interface that just holds a value of given type.
 * @param T type of value to provide
 *
 * This implementation of Provider interface just holds a value of given type as a member.
 */
template<typename T>
class SimpleProvider : public Provider<T>
{
	T Value;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance of SimpleProvider with default value to provide.
	 * @param value value that will be provided
	 */
	explicit SimpleProvider(T value) : Value(value) {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Update provded value.
	 * @param value value that will be provided
	 */
	void provideValue(T value)
	{
		Value = value;
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return value set in constructor or in provideValue method.
	 */
	virtual T provide() const
	{
		return Value;
	}

};

/**
 * @}
 */

#endif // SIMPLE_PROVIDER_H
