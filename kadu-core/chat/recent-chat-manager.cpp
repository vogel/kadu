/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"

#include "recent-chat-manager.h"

#define MAX_RECENT_CHAT_COUNT 20

RecentChatManager * RecentChatManager::Instance = 0;

RecentChatManager *  RecentChatManager::instance()
{
	if (0 == Instance)
		Instance = new RecentChatManager();

	return Instance;
}

RecentChatManager::RecentChatManager() :
		CleanUpTimer(0)
{
	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);

	configurationUpdated();
}

RecentChatManager::~RecentChatManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void RecentChatManager::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	StoragePoint *point = storage();
	QDomNodeList chatElements = point->point().childNodes();

	int count = chatElements.size();

	// load all chats, because byUuid does not do that
	ChatManager::instance()->ensureLoaded();

	// itereting backwards, because addRecentChats adds chats
	// at begining of list, not at the end
	for (int i = count - 1; i >= 0; i--)
	{
		QDomElement element = chatElements.at(i).toElement();
		if (element.isNull())
			continue;

		QString uuid = element.text();
		Chat chat = ChatManager::instance()->byUuid(uuid, false);
		if (chat)
			addRecentChat(chat);
	}
}

void RecentChatManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	StoragePoint *point = storage();
	XmlConfigFile *file = point->storage();
	QDomElement mainElement = point->point().toElement();
	if (mainElement.isNull())
		return;

	QDomNodeList chatElements = point->point().childNodes();

	int count = chatElements.size();
	for (int i = 0; i < count; i++)
		mainElement.removeChild(chatElements.at(i));

	if (config_file.readBoolEntry("Chat", "RecentChatsStore", false))
		foreach (Chat chat, RecentChats)
			if (chat && !chat.uuid().isNull())
				file->appendTextNode(mainElement, "Chat", chat.uuid().toString());
}

QList<Chat> RecentChatManager::recentChats()
{
	ensureLoaded();
	return RecentChats;
}

void RecentChatManager::addRecentChat(Chat chat)
{
	if (!chat)
		return;

	ensureLoaded();
	removeRecentChat(chat);

	QDateTime *recentChatData = chat.data()->moduleData<QDateTime>("recent-chat", true);
	*recentChatData = QDateTime::currentDateTime();

	emit recentChatAboutToBeAdded(chat);
	RecentChats.prepend(chat);
	emit recentChatAdded(chat);

	// limit
	while (RecentChats.count() > MAX_RECENT_CHAT_COUNT)
		removeRecentChat(RecentChats.last());
}

void RecentChatManager::removeRecentChat(Chat chat)
{
	ensureLoaded();
	if (!RecentChats.contains(chat))
		return;

	emit recentChatAboutToBeRemoved(chat);
	RecentChats.removeAll(chat);
	emit recentChatRemoved(chat);
}

void RecentChatManager::configurationUpdated()
{
	if (config_file.readBoolEntry("Chat", "RecentChatsStore", false))
	{
		delete CleanUpTimer;
		CleanUpTimer = 0;
		return;
	}

	if (CleanUpTimer)
		return;

	CleanUpTimer = new QTimer(this);
	CleanUpTimer->setInterval(30 * 1000);
	connect(CleanUpTimer, SIGNAL(timeout()), this, SLOT(cleanUp()));
	CleanUpTimer->start();
}

void RecentChatManager::cleanUp()
{
	if (config_file.readBoolEntry("Chat", "RecentChatsStore", false))
		return;

	int secs = config_file.readNumEntry("Chat", "RecentChatsTimeout") * 60;
	QDateTime now = QDateTime::currentDateTime();

	QList<Chat> toRemove;
	foreach (Chat chat, RecentChats)
	{
		QDateTime *recentChatData = chat.data()->moduleData<QDateTime>("recent-chat");
		if (!recentChatData)
		{
			toRemove.append(chat);
			continue;
		}

		if (recentChatData->addSecs(secs) < now)
			toRemove.append(chat);
	}

	foreach (Chat chat, toRemove)
		removeRecentChat(chat);
}
