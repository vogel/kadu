/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_TYPE_H
#define STATUS_TYPE_H

#include <QtCore/QVariant>

/**
 * @addtogroup Status
 * @{
 */

/**
 * @enum StatusType
 * @author Rafał 'Vogel' Malinowski
 * @short Status type enum.
 * @see StatusTypeGroup
 *
 * This enum describes possible types of statuses.
 */
enum StatusType
{
	/**
	 * Empty (invalid) status type.
	 */
	StatusTypeNone = -1,
	/**
	 * Free for chat status type.
	 */
	StatusTypeFreeForChat = 0,
	/**
	 * Online status type.
	 */
	StatusTypeOnline = 10,
	/**
	 * Away status type.
	 */
	StatusTypeAway = 20,
	/**
	 * Not available status type.
	 */
	StatusTypeNotAvailable = 30,
	/**
	 * Do not disturb status type.
	 */
	StatusTypeDoNotDisturb = 40,
	/**
	 * Invisible status type.
	 */
	StatusTypeInvisible = 50,
	/**
	 * Offline status type.
	 */
	StatusTypeOffline = 60
};

/**
 * @addtogroup Status
 * @}
 */

Q_DECLARE_METATYPE(StatusType)

#endif // STATUS_TYPE_H
