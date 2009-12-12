/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "chat/aggregate-chat.h"
#include "protocols/protocol.h"

#include "chat/chat-aggregator-builder.h"
#include "chat/chat-manager.h"

ChatAggregatorBuilder::ChatAggregatorBuilder()
{
}

ChatAggregatorBuilder::~ChatAggregatorBuilder()
{
}

Chat ChatAggregatorBuilder::buildAggregateChat(BuddySet buddies)
{
	QList<Chat> chats;
	foreach (Chat chat, ChatManager::instance()->allItems())
	{
		if (chat.contacts().toBuddySet() == buddies)
			chats.append(chat);
	}

	if (!chats.isEmpty())
		return AggregateChat(chats);
}
