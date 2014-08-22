/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ROSTER_ENTRY_H
#define ROSTER_ENTRY_H

#include <QtCore/QObject>

#include "misc/change-notifier.h"
#include "protocols/services/roster/roster-entry-state.h"
#include "exports.h"

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterEntry
 * @author Rafał 'Vogel' Malinowski
 * @short Entry in local and remote roster.
 *
 * This class describes entry in local and remote roster and its synchronization state. It also has two flags. When Detached flag is true,
 * no synchronization of data is made between local and remote roster. When Deleted flag is true, the entry is assumed to be deleted from remote
 * roster.
 */
class KADUAPI RosterEntry : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RosterEntry)

	RosterEntryState State;
	bool Detached;
	bool RemotelyDeleted;
	ChangeNotifier MyChangeNotifier;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new RosterEntry instance.
	 * @param parent QObject parent of new instance
	 *
	 * New instance has Detached property set to false and RosterEntryUnkown synchronization state.
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
	 * @short Set new value of Detached property.
	 * @param detached new value of Detached property
	 */
	void setDetached(bool detached);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get current value of Detached property.
	 * @return current value of Detached property
	 */
	bool detached() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set new value of RemotelyDeleted property.
	 * @param remotelyDeleted new value of RemotelyDeleted property
	 */
	void setRemotelyDeleted(bool remotelyDeleted);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get current value of RemotelyDeleted property.
	 * @return current value of RemotelyDeleted property
	 */
	bool remotelyDeleted() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Get change notifier for this object.
	 * @return change notifier of this object
	 *
	 * Each time a property of this object changes returned ChangeNotifier will emit changed() signal.
	 */
	ChangeNotifier & changeNotifier();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if this entry requires synchronization with server.
	 * @return true if this entry requires synchronization with server
	 *
	 * This method returns true only if state is RosterEntryDesynchronized and this entry is not Detached.
	 */
	bool requiresSynchronization() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Check if this entry can be updated by remote roster.
	 * @return true if this entry can be updated by remote roster
	 *
	 * Always return false for detached entries.
	 * If entry is not detached then this method returns true only if this entry is synchronized (or state is unknown).
	 * For RosterEntryDesynchronized or RosterEntrySynchronizing entries remote changes cannot be accepted with assumption
	 * that local changes are more important and will overwrite remote ones.
	 */
	bool canAcceptRemoteUpdate() const;

};

/**
 * @}
 */

#endif // ROSTER_ENTRY_H
