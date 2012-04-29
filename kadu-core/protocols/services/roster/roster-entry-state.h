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

#ifndef ROSTER_ENTRY_STATE_H
#define ROSTER_ENTRY_STATE_H

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterEntryState
 * @author Rafał 'Vogel' Malinowski
 * @short Description of state of roster entry.
 *
 * This enum describes state of roster entry that can be either synchronized, desynchronized or being synchronizing with remote entry.
 */
enum RosterEntryState
{
	/**
	 * Unkown, not yet set state.
	 */
	RosterEntryUnknown,
	/**
	 * Entries with this state have the same value on local and remote roster. When remote roster sends an update then local value must be updated as well.
	 */
	RosterEntrySynchronized,
	/**
	 * Entries with this state have different value on local and remote roster. At first opportunity new value must be sent to remote roster.
	 */
	RosterEntryDesynchronized,
	/**
	 * Entries with this state are being synchronized with remote server.
	 */
	RosterEntrySynchronizing
};

/**
 * @}
 */

#endif // ROSTER_ENTRY_STATE_H
