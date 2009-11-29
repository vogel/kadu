/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/chat-manager.h"

ChatManager * ChatManager::Instance = 0;

ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
		Instance = new ChatManager();

	return Instance;
}

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
}

void ChatManager::itemAboutToBeAdded(Chat item)
{
	connect(item, SIGNAL(chatTypeLoaded()), this, SLOT(chatTypeLoaded()));
	connect(item, SIGNAL(chatTypeUnloaded()), this, SLOT(chatTypeUnloaded()));
}

void ChatManager::itemAboutToBeRemoved(Chat item)
{
	disconnect(item, SIGNAL(chatTypeLoaded()), this, SLOT(chatTypeLoaded()));
	disconnect(item, SIGNAL(chatTypeUnloaded()), this, SLOT(chatTypeUnloaded()));
}

void ChatManager::itemAboutToBeRegistered(Chat item)
{
	emit chatAboutToBeAdded(item);
}

void ChatManager::itemRegisterd(Chat item)
{
	emit chatAdded(item);
}

void ChatManager::itemAboutToBeUnregisterd(Chat item)
{
	emit chatAboutToBeRemoved(item);
}

void ChatManager::itemUnregistered(Chat item)
{
	emit chatRemoved(item);
}

void ChatManager::detailsLoaded(Chat chat)
{
	if (!chat.isNull())
		registerItem(chat);
}

void ChatManager::detailsUnloaded(Chat chat)
{
	if (!chat.isNull())
		unregisterItem(chat);
}
