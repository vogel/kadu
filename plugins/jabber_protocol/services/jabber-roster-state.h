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

/**
 * @enum JabberRosterState
 * @short State of jabbber roster service.
 */
enum class JabberRosterState {
	/**
	 * Roster service was not initialized and cannot perform any operation except prepareRoster().
	 */
	NonInitialized,
	/**
	 * Roster service is during prepareRoster() operation.
	 */
	Initializing,
	/**
	 * Roster is initialized and ready to accept local or remote changes.
	 */
	Initialized
};
