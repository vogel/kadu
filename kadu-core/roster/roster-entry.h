/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "misc/change-notifier.h"
#include "exports.h"

#include <QtCore/QObject>

enum class RosterEntryState;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @enum RosterEntry
 * @short Entry in local and remote roster.
 *
 * This class describes entry in local and remote roster and its synchronization state. It also has two flags. When Detached flag is true,
 * no synchronization of data is made between local and remote roster. However, deleting detached entries is reflected both locally
 * and remotely.
 */
class KADUAPI RosterEntry : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RosterEntry)

public:
	/**
	 * @short Create new RosterEntry instance.
	 * @param parent QObject parent of new instance
	 *
	 * New instance has RosterEntryUnkown synchronization state.
	 */
	explicit RosterEntry(QObject *parent = nullptr);
	virtual ~RosterEntry();

	/**
	 * @short Fixup state when initializing roster.
	 *
	 * Call after initializing roster. Roster state can be invalid if protocol was disconnected during
	 * roster operation. Entries with state RosterEntryState::SynchronizingToRemote to
	 * RosterEntryState::HasLocalChanges. It forces Roster code to synchronize to remote again.
	 *
	 * This method does not invoke changeNotifier() signals.
	 */
	void fixupInitialState();

	/**
	 * @return current value of State property
	 */
	RosterEntryState state() const;

	/**
	 * @short Set state to Synchronized.
	 */
	void setSynchronized();

	/**
	 * @short Set state to HasLocalChanges
	 * @return true if change was successfull
	 *
	 * Does nothing if state is SynchronizingToRemote or SynchronizingFromRemote or Detached. It means we
	 * can lost some local changes in some scenerios. Not very serious issue, should occur very rarely.
	 * Fix for that would be hard and may be not worth it.
	 *
	 * If succeeds, emits hasLocalChangesNotifier().
	 */
	bool setHasLocalChanges();

	/**
	 * @short Set state to SynchronizingToRemote.
	 */
	void setSynchronizingToRemote();

	/**
	 * @short Set state to SynchronizingFromRemote.
	 */
	void setSynchronizingFromRemote();

	/**
	 * @short Set state to Detached.
	 */
	void setDetached();

	/**
	 * @return has local changes notifier of this object
	 * @todo find out if bare signal would do here
	 *
	 * This change notifier is called when state is set to RosterEntryState::HasLocalChanges.
	 */
	ChangeNotifier & hasLocalChangesNotifier();

	/**
	 * @return true if this entry requires synchronization with server
	 *
	 * This method returns true only if state is HasLocalChanges.
	 */
	bool requiresSynchronization() const;

private:
	RosterEntryState m_state;
	ChangeNotifier m_hasLocalChangesNotifier;

	/**
	 * @short Set new value of State property.
	 * @param state new value of State property
	 *
	 * Emits signals from hasLocalChangesNotifier() if new state if RosterEntryState::HasLocalChanges and previous was different.
	 */
	void setState(RosterEntryState state);

};

/**
 * @}
 */
