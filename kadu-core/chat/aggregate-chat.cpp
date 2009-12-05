/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-set-configuration-helper.h"

#include "chat/aggregate-chat.h"

AggregateChat::AggregateChat(Chat chat) : 
		Chat(chat)
{
	Chats.append(chat);
}

AggregateChat::AggregateChat(QList<Chat> chats) :
		Chat(chats.at(0)), Chats(chats)
{
}

AggregateChat::~AggregateChat()
{
}

void AggregateChat::load()
{
}

void AggregateChat::store()
{
}

void AggregateChat::addChat(Chat chat)
{
	Chats.append(chat);
}

void AggregateChat::removeChat(Chat chat)
{
	if (Chats.size() == 1)
		return;

	foreach (Chat c, Chats)
	if (c.uuid() == chat.uuid())
		Chats.removeAll(c);
}
