/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MESSAGE_COMMON_H
#define MESSAGE_COMMON_H

// MessageStatus enum vaules identical with those defined in kadu-core/chat-style/engine/chat-scripts.js
enum MessageStatus
{
	MessageStatusUnknown = 0,
	MessageStatusDelivered = 4,
	MessageStatusWontDeliver = 5
};

enum MessageType
{
	MessageTypeUnknown = 0,
	MessageTypeReceived = 1,
	MessageTypeSent = 2,
	MessageTypeSystem = 3
};

#endif // MESSAGE_COMMON_H
