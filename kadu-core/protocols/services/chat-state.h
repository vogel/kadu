/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 * @enum State
 * @short This enum represent state of chat.
 * @note Values in this enum must be identical with those defined in kadu-core/chat-style/engine/chat-scripts.js
 */
enum class ChatState
{
	/**
	 * Chat is active - window is opened and active.
	 */
	Active = 0,
	/**
	 * Chat is in compising state - new message is being entered, but not send yet.
	 */
	Composing = 1,
	/**
	 * Chat is gone - window was closed.
	 */
	Gone = 2,
	/**
	 * Chat is inactive - window is opened but not active.
	 */
	Inactive = 3,
	/**
	 * Unknown state.
	 */
	None = 4,
	/**
	 * Chat is in paused state - new message is written, but entering was paused.
	 */
	Paused = 5
};
