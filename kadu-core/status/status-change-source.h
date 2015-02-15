/*
 * %kadu copyright begin%
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

#ifndef STATUS_CHANGE_SOURCE_H
#define STATUS_CHANGE_SOURCE_H

/**
 * @addtogroup Status
 * @{
 */

/**
 * @enum StatusChangeSource
 * @author Rafał 'Vogel' Malinowski
 * @short Describe source of given status change.
 * @see StatatusChanger
 */
enum StatusChangeSource
{
	/**
	 * Status change was initiated by end user.
	 */
	SourceUser,
	/**
	 * Status change was initiated by instance of StatatusChanger class.
	 */
	SourceStatusChanger
};

/**
 * @addtogroup Status
 * @}
 */

#endif // STATUS_CHANGE_SOURCE_H
