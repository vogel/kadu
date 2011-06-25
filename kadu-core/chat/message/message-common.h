/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
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

// MessageStatus enum vaules identical with those defined in kadu-core/chat/style-engines/chat-scripts.js
enum MessageStatus
{
	MessageStatusUnknown = 0,
	MessageStatusReceived = 1,
	MessageStatusSent = 2,
	MessageStatusDelivered = 3,
	MessageStatusWontDeliver = 4,
};

enum MessageType
{
	MessageTypeUnknown,
	MessageTypeReceived,
	MessageTypeSent,
	MessageTypeSystem,
};

#endif // MESSAGE_COMMON_H
