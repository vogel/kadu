/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MANAGER_COMMON_H
#define MANAGER_COMMON_H

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @enum NotFoundAction
 * @author Rafal 'Vogel' Malinowski
 *
 * Action performed by manager when requested item was not found.
 */
enum NotFoundAction
{
	/**
	 * @author Rafal 'Vogel' Malinowski
	 *
	 * When item is not found, Item::null value will be returned.
	 */
	ActionReturnNull,
	/**
	 * @author Rafal 'Vogel' Malinowski
	 *
	 * When item is not found, new item with given data will be created and returned.
	 */
	ActionCreate,
	/**
	 * @author Rafal 'Vogel' Malinowski
	 *
	 * When item is not found, new item with given data will be created,
	 * added to manager and returned.
	 */
	ActionCreateAndAdd
};

/**
 * @}
 */

#endif // MANAGER_COMMON_H
