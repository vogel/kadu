/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "icons-manager.h"

#include "chat-type-manager.h"

ChatTypeManager * ChatTypeManager::Instance = 0;

ChatTypeManager * ChatTypeManager::instance()
{
	if (0 == Instance)
		Instance = new ChatTypeManager();

	return Instance;
}

ChatTypeManager::ChatTypeManager()
{
	// TODO: 0.6.6 fix icons
	addChatType(ChatType(0, "SimpleChat", tr("Chat"), IconsManager::instance()->loadIcon("OpenChat")));
	addChatType(ChatType(1, "ConferenceChat", tr("Conference"), IconsManager::instance()->loadIcon("ManageModules")));
}

ChatTypeManager::~ChatTypeManager()
{
}

void ChatTypeManager::addChatType(ChatType chatType)
{
	if (ChatTypes.contains(chatType))
		return;

	emit chatTypeAboutToBeAdded(chatType);
	ChatTypes.append(chatType);
	ChatTypesMap.insert(chatType.name(), chatType);
	emit chatTypeAdded(chatType);
}

void ChatTypeManager::removeChatType(ChatType chatType)
{
	if (!ChatTypes.contains(chatType))
		return;
	
	emit chatTypeAboutToBeRemoved(chatType);
	ChatTypes.removeAll(chatType);
	ChatTypesMap.remove(chatType.name());
	emit chatTypeRemoved(chatType);
}

ChatType ChatTypeManager::chatType(const QString &name)
{
	return ChatTypesMap.value(name);
}
