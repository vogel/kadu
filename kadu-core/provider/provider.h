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

#ifndef PROVIDER_H
#define PROVIDER_H

/**
 * @addtogroup Provider
 * @{
 */

/**
 * @class Provider
 * @author Rafał 'Vogel' Malinowski
 * @short Interface used to get instance of given type.
 * @param T type of value to provide
 *
 * This interface should be used in places where an existing object of given type is required.
 */
template<typename T>
class Provider
{
public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Provide value of given type.
	 */
	virtual T provide() const = 0;
	virtual ~Provider() {}

};

/**
 * @}
 */

#endif // PROVIDER_H
