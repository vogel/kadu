/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_TYPE_GROUP_H
#define STATUS_TYPE_GROUP_H

/**
 * @addtogroup Status
 * @{
 */

/**
 * @enum StatusTypeGroup
 * @author Rafał 'Vogel' Malinowski
 * @short Status type grouping enum.
 * @see StatusType
 *
 * This enum allows grouping of status types.
 */
enum StatusTypeGroup
{
	/**
	 * Empty (invalid) status type group.
	 */
	StatusTypeGroupNone = -1,
	/**
	 * Online status type group. Contains "Online" and "Free for chat" status types.
	 */
	StatusTypeGroupOnline = 0,
	/**
	 * Away status type group. Contains "Away", "Not available" and "Do not disturb" status types.
	 */
	StatusTypeGroupAway = 10,
	/**
	 * Invisible status type group. Contains "Invisible" status type.
	 */
	StatusTypeGroupInvisible = 20,
	/**
	 * Offline status type group. Contains "Offline" status type.
	 */
	StatusTypeGroupOffline = 30
};

/**
 * @addtogroup Status
 * @}
 */

#endif // STATUS_TYPE_GROUP_H
