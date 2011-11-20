/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef STATUS_H
#define STATUS_H

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "status/status-type-group.h"
#include "status/status-type.h"

#include "exports.h"

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class Status
 * @author Rafał 'Vogel' Malinowski
 * @short Class representing current status of user.
 * @see StatusType
 * @see StatusTypeGroup
 *
 * This class describes current status of any user. Status contains two values - status type and description.
 * Statuses can be ordered using StatusType enum (smaller value means greater availability).
 */
class KADUAPI Status
{
	StatusType Type;
	StatusTypeGroup Group;
	QString Description;
	QString DisplayName;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new status with given type and description.
	 * @param statusType type of new status
	 * @param description description of new status.
	 *
	 * Creates new status with given type and description.
	 */
	Status(StatusType statusType = StatusTypeOffline, const QString &description = QString());
	Status(const Status &copyme);
	~Status();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns this status type.
	 * @return this status type
	 *
	 * Returns this status type.
	 */
	StatusType type() const { return Type; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates this status type.
	 * @param type new status type
	 *
	 * Updates this status type. Values of displayName and group are also updated.
	 */
	void setType(StatusType type);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns this status type group.
	 * @return this status type group
	 *
	 * Returns this status type group. This value depends only on current status type.
	 */
	StatusTypeGroup group() const { return Group; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns this status display name.
	 * @return this status display name
	 *
	 * Returns this status display name. This value depends only on current status type.
	 */
	const QString & displayName() const { return DisplayName; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns this status description.
	 * @return this status description
	 *
	 * Returns this status description.
	 */
	const QString & description() const { return Description; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates this status description.
	 * @param escription new status description
	 *
	 * Updates this status description.
	 */
	void setDescription(const QString &description) { Description = description; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if this status description is not empty.
	 * @return true if this status description is not empty
	 *
	 * Returns true if this status description is not empty.
	 */
	bool hasDescription() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if this status if not connected (offline).
	 * @return true if this status if not connected (offline)
	 *
	 * Returns true if this status if not connected (offline).
	 */
	bool isDisconnected() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two statuses.
	 * @return true if left status is more available than right one
	 *
	 * This method compares two statuses. Status which is more available is considered 'less', so it returns
	 * true when left status is more available than right one.
	 */
	bool operator < (const Status &compare) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two statuses.
	 * @return true if both statuses are equal
	 *
	 * This method compares two statuses. Statuses are equal when its types and description are equal.
	 */
	bool operator == (const Status &compare) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Compares two statuses.
	 * @return true if statuses are not equal
	 *
	 * This method compares two statuses. Statuses are not equal when its types or description are not equal.
	 */
	bool operator != (const Status &compare) const;

};

/**
 * @addtogroup Status
 * @}
 */

Q_DECLARE_METATYPE(Status)

#endif // STATUS_H
