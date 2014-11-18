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

#pragma once

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @enum RosterEntryState
 * @short Description of state of roster entry.
 *
 * This enum describes state of roster entry that can be either synchronized, desynchronized or being synchronizing with remote entry.
 */
enum class RosterEntryState
{
	/**
	 * Unkown, not yet set state.
	 */
	Unknown,
	/**
	 * Entries with this state have the same value on local and remote roster. When remote roster sends an update then local value must be updated as well.
	 */
	Synchronized,
	/**
	 * Entries with this state have local changes and should be sent to remote server (if not detached).
	 */
	HasLocalChanges,
	/**
	 * Entries with this state are being synchronized to remote server.
	 */
	SynchronizingToRemote,
	/**
	 * Entries with this state are being synchronized from remote server.
	 */
	SynchronizingFromRemote,
	/**
	 * Entries with this state are not synchronized at all with remote server. However, when one of remote or local rosters removes them
	 * then on second roster it will be removed as well.
	 */
	Detached
};

/**
 * @}
 */
