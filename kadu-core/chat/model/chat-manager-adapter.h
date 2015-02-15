/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_MANAGER_ADAPTER_H
#define CHAT_MANAGER_ADAPTER_H

#include <QtCore/QObject>

#include "chat/chat.h"

class ChatListModel;

/**
 * @addtogroup Buddy
 * @{
 */

/**
 * @class ChatManagerAdapter
 * @author Rafał 'Vogel' Malinowski
 * @short Adapter that makes ChatListModel to use copy of buddies from ChatManager.
 *
 * This adapter created with a @link ChatListModel @endlink as parent replicates list of chats from
 * @link ChatManager @endlink on this model. After creation other, non-managable, chats can be
 * added and removed from @link ChatListModel @endlink. Removing or adding managable chats is undefined.
 */
class ChatManagerAdapter : public QObject
{
	Q_OBJECT

	ChatListModel *Model;

private slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called after a chat is added to @link ChatManager @endlink singleton.
	 * @param chat added chat
	 *
	 * Chat is added to @link ChatListModel @endlink instance.
	 */
	void chatAdded(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called after a chat is removed from @link ChatManager @endlink singleton.
	 * @param chat removed chat
	 *
	 * Chat is removed from @link ChatListModel @endlink instance.
	 */
	void chatRemoved(const Chat &chat);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new BuddyManagerAdapter on given @link BuddyListModel @endlink.
	 * @param model model to adapt @link BuddiesManager @endlink to
	 *
	 * Given @link BuddyListModel @endlink will now have exactly the same data as @link BuddiesManager @endlink singleton.
	 * Non-managable buddies can be added or removed from this model.
	 */
	explicit ChatManagerAdapter(ChatListModel *model);
	virtual ~ChatManagerAdapter();

};

/**
 * @}
 */

#endif // CHAT_MANAGER_ADAPTER_H
