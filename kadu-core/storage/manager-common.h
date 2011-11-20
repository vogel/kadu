/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
