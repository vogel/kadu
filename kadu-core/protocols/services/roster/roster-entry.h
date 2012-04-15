/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ROSTER_ENTRY_H
#define ROSTER_ENTRY_H

#include <QtCore/QObject>

#include "protocols/services/roster/roster-entry-status.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterEntry
 * @author Rafał 'Vogel' Malinowski
 * @short Entry in local and remote roster.
 * 
 * This class describes entry in local and remote roster and its synchronization status.
 */
class RosterEntry : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RosterEntry)
	
	RosterEntryStatus Status;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new RosterEntry instance.
	 * @param paretn QObject parent of new instance
	 */
	explicit RosterEntry(QObject *parent = 0);
	virtual ~RosterEntry();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set new value of Status property.
	 * @param status new value of Status property
	 */
	void setStatus(RosterEntryStatus status);
	
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get current value of Status property.
	 * @return Current value of Status property.
	 */
	RosterEntryStatus status() const;

};

/**
 * @}
 */

#endif // ROSTER_ENTRY_H
