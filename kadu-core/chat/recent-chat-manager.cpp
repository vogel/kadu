/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QVector>

#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "message/message-manager.h"
#include "message/message.h"

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
	{
		Instance = new RecentChatManager();
		Instance->init();
	}

	return Instance;
}

RecentChatManager::RecentChatManager()
{
}

RecentChatManager::~RecentChatManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void RecentChatManager::init()
{
	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);

	CleanUpTimer.setInterval(30 * 1000);
	connect(&CleanUpTimer, SIGNAL(timeout()), this, SLOT(cleanUp()));

	configurationUpdated();

	connect(MessageManager::instance(), SIGNAL(messageReceived(Message)),
			this, SLOT(onNewMessage(Message)));
	connect(MessageManager::instance(), SIGNAL(messageSent(Message)),
			this, SLOT(onNewMessage(Message)));
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

	QSharedPointer<StoragePoint> point(storage());
	QDomNodeList chatElements = point->point().childNodes();

	int count = chatElements.size();

	// load all chats, because byUuid does not do that
	ChatManager::instance()->ensureLoaded();

	// itereting backwards, because addRecentChats adds chats
	// at beginning of list, not at the end
	for (int i = count - 1; i >= 0; i--)
	{
		QDomElement element = chatElements.at(i).toElement();
		if (element.isNull())
			continue;

		QString uuid = element.attribute("uuid");
		int time = element.attribute("time").toInt();
		Chat chat = ChatManager::instance()->byUuid(uuid);
		if (chat)
		{
			QDateTime datetime;
			datetime.setTime_t(time);
			addRecentChat(chat, datetime);
		}
	}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Stores recent chats data into configuration.
 *
 * Stores recent chats data into configuration. Chats are stored as list
 * of uuids and times. First chat in the list is the most rectent chat.
 */
void RecentChatManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	StorableObject::store();

	QSharedPointer<StoragePoint> point(storage());
	QDomElement mainElement = point->point().toElement();
	if (mainElement.isNull())
		return;

	QDomNodeList chatElements = point->point().childNodes();

	int count = chatElements.size();
	for (int i = 0; i < count; i++)
		mainElement.removeChild(chatElements.at(i));

	if (!config_file.readBoolEntry("Chat", "RecentChatsClear", false))
		foreach (const Chat &chat, RecentChats)
			if (chat && !chat.uuid().isNull())
			{
				QDomElement chatelement = point->point().ownerDocument().createElement("Chat");
				chatelement.setAttribute("time", chat.property("recent-chat:dateTime", QDateTime()).toDateTime().toTime_t());
				chatelement.setAttribute("uuid", chat.uuid().toString());
				mainElement.appendChild(chatelement);
			}
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Return list of recent chats.
 * @return list of recent chats
 *
 * Returns list of recent chats sorted from most recent to least recent.
 */
const QList<Chat> & RecentChatManager::recentChats()
{
	ensureLoaded();
	return RecentChats;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Adds new chat to list (or moves it to the first position).
 *
 * Adds new chat to list. If chat is already on list it is just moved to the first place.
 * Else, it is added at first place and all chats after 20th are removed. Time of add is
 * assigned to chat variable, so it can be used to remove chat after configured amount
 * of time.
 *
 * Signals recentChatAboutToBeAdded and recentChatAdded are emitted.
 * If the chat was on the list on the first position already, signals are NOT emitted.
 */
void RecentChatManager::addRecentChat(Chat chat, QDateTime datetime)
{
	if (!chat)
		return;

	ensureLoaded();

	chat.addProperty("recent-chat:dateTime", datetime, CustomProperties::NonStorable);

	if (!RecentChats.isEmpty() && RecentChats.at(0) == chat)
		return;

	removeRecentChat(chat);

	// limit
	while (RecentChats.count() >= MAX_RECENT_CHAT_COUNT)
		removeRecentChat(RecentChats.last());

	emit recentChatAboutToBeAdded(chat);
	RecentChats.prepend(chat);
	emit recentChatAdded(chat);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes chat from list.
 *
 * Removes given chat from list.
 *
 * Signals recentChatAboutToBeRemoved and recentChatRemoved are emitted.
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
 * If RecentChatsClear is changed to false this manager will not remove chats
 * from the list by itself (afer RecentChatsTimeout timeout). All recent chats
 * are stored and restored between program launches.
 *
 * If RecentChatsClear is changed to true this manager will remove chats from
 * the list by itself on program exit.
 */
void RecentChatManager::configurationUpdated()
{
	CleanUpTimer.stop();
	RecentChatsTimeout = config_file.readNumEntry("Chat", "RecentChatsTimeout") * 60;
	if (RecentChatsTimeout > 0)
		CleanUpTimer.start();

	QTimer::singleShot(0, this, SLOT(cleanUp()));
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Removes too old recent chats.
 *
 * If RecentChatsTimeout is greater than 0, this method will remove
 * all chats that were added before RecentChatsTimeout minutes ago.
 */
void RecentChatManager::cleanUp()
{
	ensureLoaded();

	if (RecentChatsTimeout <= 0)
		return;

	QDateTime now = QDateTime::currentDateTime();

	foreach (const Chat &chat, RecentChats)
	{
		if (chat.hasProperty("recent-chat:dateTime") &&
		    chat.property("recent-chat:dateTime", QDateTime()).toDateTime().addSecs(RecentChatsTimeout) < now)
			removeRecentChat(chat);
	}
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Adds given message's chat to the list.
 *
 * Called every time a new message is sent or received. Adds that message's
 * chat to the list with addRecentChat method.
 */
void RecentChatManager::onNewMessage(const Message &message)
{
	addRecentChat(message.messageChat());
}
