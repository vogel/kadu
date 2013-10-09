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

#ifndef ROSTER_TASK_H
#define ROSTER_TASK_H

#include <QtCore/QString>

#include "protocols/services/roster/roster-task-type.h"
#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class RosterTask
 * @author Rafał 'Vogel' Malinowski
 * @short Class describing one roster task.
 *
 * Roster task is described by contact id (username) and roster task type. More data about task is read from Kadu's
 * @link ContactManager @endlink singleton.
 */
class KADUAPI RosterTask
{
	RosterTaskType Type;
	QString Id;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create empty instance of RosterTask class.
	 */
	RosterTask();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of RosterTask class.
	 * @param type type of roster task
	 * @param id id of contact
	 */
	RosterTask(RosterTaskType type, const QString &id);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Copy existing instance of RosterTask class.
	 * @param copyMe instance to copy
	 */
	RosterTask(const RosterTask &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Copy existing instance of RosterTask class.
	 * @param copyMe instance to copy
	 */
	RosterTask & operator = (const RosterTask &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if this instance is idenitcal to other one.
	 * @param compare instance to compare with
	 */
	bool operator == (const RosterTask &compare) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return value of Type property.
	 * @return value of Type property
	 */
	RosterTaskType type() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return value of Id property.
	 * @return value of Id property
	 */
	QString id() const;

};

/**
 * @}
 */

#endif // ROSTER_TASK_H
