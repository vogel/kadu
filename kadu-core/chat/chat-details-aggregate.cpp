/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/type/chat-type-manager.h"
#include "contacts/contact-set.h"

#include "chat-details-aggregate.h"

ChatDetailsAggregate::ChatDetailsAggregate(ChatShared *chatData) :
		ChatDetails(chatData)
{
}

ChatDetailsAggregate::~ChatDetailsAggregate()
{
}

bool ChatDetailsAggregate::shouldStore()
{
	false;
}

ChatType * ChatDetailsAggregate::type() const
{
	return ChatTypeManager::instance()->chatType("Aggregate");
}

ContactSet ChatDetailsAggregate::contacts() const
{
	return Chats.isEmpty() ? ContactSet() : Chats[0].contacts();
}

QString ChatDetailsAggregate::name() const
{
	return Chats.isEmpty() ? QString::null : Chats[0].name();
}

void ChatDetailsAggregate::setChats(QList<Chat> chats)
{
	Chats = chats;
}

QList<Chat> ChatDetailsAggregate::chats()
{
	return Chats;
}
