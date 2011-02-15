/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef RECENT_CHAT_MANAGER_H
#define RECENT_CHAT_MANAGER_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "configuration/configuration-aware-object.h"
#include "storage/storable-object.h"

class QTimer;

class Chat;
class Message;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class RecentChatManager
 * @author Rafal 'Vogel' Malinowski
 * @short Manager for recently used chats.
 *
 * This class manages recently used chats in application (maximum 20). Depending on configuration
 * it either stores and loads the recent chats between program launches or clears it at program exit.
 * Recent chats are also removed after specified amount of time.
 * This manager is used for 'Recent chats' menu item.
 *
 * Use @link addRecentChat @endlink to add new recent chat. Old recent chats are automatially
 * removed after configurable period of time or when count of items would be bigger than 20.
 * If item is already on list it will be moved to the first place.
 */
class KADUAPI RecentChatManager : public QObject, public StorableObject, private ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(RecentChatManager)

	static RecentChatManager * Instance;

	QList<Chat> RecentChats;
	QTimer CleanUpTimer;

	int RecentChatsTimeout;

	RecentChatManager();
	virtual ~RecentChatManager();

	virtual void load();

	void addRecentChat(Chat chat, QDateTime datetime = QDateTime::currentDateTime());
	void removeRecentChat(Chat chat);

private slots:
	void cleanUp();
	void onNewMessage(const Message &message);

protected:
	virtual void configurationUpdated();

public:
	static RecentChatManager * instance();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name for storage of recent chat ids.
	 * @return node name for storage of recent chat ids
	 *
	 * Returns node name for storage of recent chat ids - "RecentChat".
	 * This node is only used when configuration allows of storing that data.
	 */
	virtual QString storageNodeName() { return QLatin1String("RecentChats"); }

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns parent node for storage - the main node.
	 * @return parent node for storage - root node
	 *
	 * Returns parent node for storage - the main node.
	 */
	virtual StorableObject* storageParent() { return 0; }

	virtual void store();

	const QList<Chat> & recentChats();

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just before recent chat is added to manager.
	 * @param chat added recent chat
	 *
	 * Signal is emitted just before recent chat is added to manager.
	 */
	void recentChatAboutToBeAdded(Chat chat);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just after recent chat is added to manager.
	 * @param chat added recent chat
	 *
	 * Signal is emitted just after recent chat is added to manager.
	 */
	void recentChatAdded(Chat chat);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just before recent chat is removed from manager.
	 * @param chat added recent chat
	 *
	 * Signal is emitted just before recent chat is removed from manager.
	 */
	void recentChatAboutToBeRemoved(Chat chat);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emitted just after recent chat is removed from manager.
	 * @param chat added recent chat
	 *
	 * Signal is emitted just after recent chat is removed from manager.
	 */
	void recentChatRemoved(Chat chat);

};

/**
 * @}
 */

#include "chat/chat.h" // for MOC

#endif // RECENT_CHAT_MANAGER_H
