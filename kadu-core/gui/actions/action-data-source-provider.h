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

#ifndef ACTION_DATA_SOURCE_PROVIDER_H
#define ACTION_DATA_SOURCE_PROVIDER_H

class ActionDataSource;

/**
 * @addtogroup Actions
 * @{
 */

/**
 * @class ActionDataSourceProvider
 * @author Rafał 'Vogel' Malinowski
 * @short Interface used by classes that have access to ActionDataSource instances.
 *
 * This inteface is implemented by all classes/widgets that have access to ActionDataSource instances.
 * For example, all main windows must implement this interface.
 */
class ActionDataSourceProvider
{
public:
	virtual ActionDataSource * actionDataSource() = 0;

};

/**
 * @}
 */

#endif // ACTION_DATA_SOURCE_PROVIDER_H
