/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/type/chat-type-manager.h"

#include "chat-type-aware-object.h"

KADU_AWARE_CLASS(ChatTypeAwareObject)

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to call chatTypeRegistered in each ChatTypeAwareObject object.
 * @param chatType ChatType object that will get passed to chatTypeRegistered methods
 *
 * Calling this method results in calling chatTypeRegistered in each ChatTypeAwareObject
 * in system.
 */
void ChatTypeAwareObject::notifyChatTypeRegistered(ChatType *chatType)
{
	foreach (ChatTypeAwareObject *object, Objects)
		object->chatTypeRegistered(chatType);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to call chatTypeUnregistered in each ChatTypeAwareObject object.
 * @param chatType ChatType object that will get passed to chatTypeUnregistered methods
 *
 * Calling this method results in calling chatTypeUnregistered in each ChatTypeAwareObject
 * in system.
 */
void ChatTypeAwareObject::notifyChatTypeUnregistered(ChatType *chatType)
{
	foreach (ChatTypeAwareObject *object, Objects)
		object->chatTypeUnregistered(chatType);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to call chatTypeRegistered for each registered ChatType object.
 *
 * Calling this method results in calling chatTypeRegistered for each registered ChatType
 * on current object.
 */
void ChatTypeAwareObject::triggerAllChatTypesRegistered()
{
	foreach (ChatType *chatType, ChatTypeManager::instance()->chatTypes())
		chatTypeRegistered(chatType);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Call this method to call chatTypeUnregistered for each registered ChatType object.
 *
 * Calling this method results in calling chatTypeUnregistered for each registered ChatType
 * on current object.
 */
void ChatTypeAwareObject::triggerAllChatTypesUnregistered()
{
	foreach (ChatType *chatType, ChatTypeManager::instance()->chatTypes())
		chatTypeUnregistered(chatType);
}
