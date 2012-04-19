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

#include "protocols/services/roster/roster-entry-state.h"

class ChangeNotifier;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterEntry
 * @author Rafał 'Vogel' Malinowski
 * @short Entry in local and remote roster.
 *
 * This class describes entry in local and remote roster and its synchronization state.
 */
class RosterEntry : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RosterEntry)

	RosterEntryState State;
	ChangeNotifier *StateChangeNotifier;

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
	 * @short Set new value of State property.
	 * @param state new value of State property
	 */
	void setState(RosterEntryState state);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get current value of State property.
	 * @return current value of State property
	 */
	RosterEntryState state() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get change notifier for State property.
	 * @return change notifier of State property
	 *
	 * Use this getter to obtain change notifier object for State property. Each time State property value changes this object will
	 * emit its changed() signal.
	 */
	ChangeNotifier * stateChangeNotifier() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if this entry required synchronization with server.
	 * @return true if this entry required synchronization with server
	 *
	 * This method only returns true if state if equal to RosterEntryDirty.
	 */
	bool requiresSynchronization() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set state to Dirty/Synchronized if not Detached.
	 * @param dirty dirntess flag
	 *
	 * If State of this RosterEntry is set to RosterStateDetached nothing changes. In other cases
	 * State is set to RosterStateDirty if Dirty is true and RosterStateSynchronized otherwise.
	 *
	 * To change state from RosterStateDetached use setState() directly.
	 */
	void markDirty(bool dirty);

};

/**
 * @}
 */

#endif // ROSTER_ENTRY_H
