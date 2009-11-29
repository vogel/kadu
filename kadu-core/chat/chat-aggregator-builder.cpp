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

ChatAggregatorBuilder::ChatAggregatorBuilder()
{
}

ChatAggregatorBuilder::~ChatAggregatorBuilder()
{
}

Chat ChatAggregatorBuilder::buildAggregateChat(BuddySet contacts)
{
	QList<Chat> chats;
	foreach (Account account, AccountManager::instance()->items())
	{
		Chat chat = account.protocolHandler()->findChat(contacts, false);
		if (chat)
			chats.append(chat);
	}

// 	if (!chats.isEmpty()) enable after chat-rework
// 		return new AggregateChat(chats);
	return Chat::null;
}
