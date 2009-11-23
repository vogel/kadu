/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/type/chat-type-manager.h"

#include "chat-type-aware-object.h"

KADU_AWARE_CLASS(ChatTypeAwareObject)

void ChatTypeAwareObject::notifyChatTypeRegistered(ChatType *chatType)
{
	foreach (ChatTypeAwareObject *object, Objects)
		object->chatTypeRegistered(chatType);
}

void ChatTypeAwareObject::notifyChatTypeUnregistered(ChatType *chatType)
{
	foreach (ChatTypeAwareObject *object, Objects)
		object->chatTypeUnregistered(chatType);
}

void ChatTypeAwareObject::triggerAllChatTypesRegistered()
{
	foreach (ChatType *chatType, ChatTypeManager::instance()->chatTypes())
		chatTypeRegistered(chatType);
}

void ChatTypeAwareObject::triggerAllChatTypesUnregistered()
{
	foreach (ChatType *chatType, ChatTypeManager::instance()->chatTypes())
		chatTypeUnregistered(chatType);
}
