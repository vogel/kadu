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

#ifndef ROSTER_ENTRY_STATUS_H
#define ROSTER_ENTRY_STATUS_H

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @enum RosterEntryStatus
 * @author Rafał 'Vogel' Malinowski
 * @short Description of status of roster entry.
 * 
 * This enum describes status of roster entry that can be either synchronized with remote entry, require an update on can be detached from remote roster.
 */
enum RosterEntryStatus
{
	/**
	 * Unkown, not yet set status.
	 */
	RosterEntryUnkown,
	/**
	 * Entries with this status have the same value on local and remote roster. When remote roster sends an update then local value must be updated as well.
	 */
	RosterEntrySynchronized,
	/**
	 * Entries with this status have different value on local and remote roster. At first opportunity new value must be sent to remote roster.
	 */
	RosterEntryDirty,
	/**
	 * Entries with this status are detached from remote roster. No updated are synchronized between remote and local roster.
	 * For example, Facebook contacts can have updated Display name, but on local roster it can be changed.
	 */
	RosterEntryDetached
};

/**
 * @}
 */

#endif // ROSTER_ENTRY_STATUS_H
