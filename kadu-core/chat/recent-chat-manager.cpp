/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"

#include "recent-chat-manager.h"

#define MAX_RECENT_CHAT_COUNT 20

RecentChatManager * RecentChatManager::Instance = 0;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Returns singleton instance of RecentChatManager.
 * @return singleton instance of RecentChatManager
 */
RecentChatManager * RecentChatManager::instance()
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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Loads recent chats data from configuration.
 *
 * Loads recent chats data from configuration. Chats are stored as list
 * of uuids. First chat in the list is the most rectent chat.
 */
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
		Chat chat = ChatManager::instance()->byUuid(uuid);
		if (chat)
			addRecentChat(chat);
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores recent chats data into configuration.
 *
 * Stores recent chats data into configuration. Chats are stored as list
 * of uuids. First chat in the list is the most rectent chat.
 */
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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Return list of recent chats.
 * @return list of recent chats
 *
 * Returns list of recent chats sorted from most recent to least recent.
 */
QList<Chat> RecentChatManager::recentChats()
{
	ensureLoaded();
	return RecentChats;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Adds new chat to list (or moves it on first position).
 *
 * Adds new chat to list. If chat is already on list it is just moved to the first place.
 * Else, it is added at first place and all chats after 20th are removed. Time of add is
 * assigned to chat variable, so it can be used to remove chat after configured amount
 * of time.
 *
 * Signals recentChatAboutToBeAdded and recentChatAdded are emited.
 */
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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes chat from list.
 *
 * Removes given chat from list.
 *
 * Signals recentChatAboutToBeRemoved and recentChatRemoved are emited.
 */
void RecentChatManager::removeRecentChat(Chat chat)
{
	ensureLoaded();
	if (!RecentChats.contains(chat))
		return;

	emit recentChatAboutToBeRemoved(chat);
	RecentChats.removeAll(chat);
	emit recentChatRemoved(chat);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Updates behaviour of manager to new configuration values.
 *
 * If RecentChatsStore is changed to true this manager will not try to remove
 * chats from list by itself (afer RecentChatsTimeout timeout). All recent chats
 * are stored and restored between program runs.
 *
 * If RecentChatsStore is changed to false this manager will try to remove
 * chats from list by itself afer RecentChatsTimeout timeout.
 */
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

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes too old recent chats.
 *
 * If RecentChatsStore configuration value is false this method will remove
 * all chats that were added before RecentChatsTimeout minutes ago.
 */
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
