/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "protocols/protocol.h"

#include "chat/aggregate-chat-builder.h"
#include "chat/chat-manager.h"
#include "chat-details-aggregate.h"

Chat AggregateChatBuilder::buildAggregateChat(BuddySet buddies)
{
	QList<Chat> chats;
	foreach (Chat chat, ChatManager::instance()->allItems())
		if (chat.contacts().toBuddySet() == buddies)
			chats.append(chat);

	if (chats.size() <= 1)
		return Chat::null;

	Chat result = Chat::create();
	result.setType(chats[0].type());

	ChatDetailsAggregate *details = new ChatDetailsAggregate(result);
	details->setChats(chats);
	result.setDetails(details);

	return result;
}
