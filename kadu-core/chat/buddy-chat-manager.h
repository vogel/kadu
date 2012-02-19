/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef BUDDY_CHAT_MANAGER_H
#define BUDDY_CHAT_MANAGER_H

#include <QtCore/QHash>
#include <QtCore/QObject>

#include "chat/chat.h"
#include "exports.h"

class BuddySet;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class BuddyChatManager
 * @short Makes chat object that buddys all chats for given buddy set.
 *
 * Class contains one static method that makes chat object that
 * buddys all chats for given buddy set.
 */
class KADUAPI BuddyChatManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyChatManager)

	static BuddyChatManager *Instance;

	QHash<BuddySet, QVector<Chat> > BuddyChats;

	BuddyChatManager();
	virtual ~BuddyChatManager();

	void init();

private slots:
	void chatAdded(const Chat &chat);
	void chatRemoved(const Chat &chat);

public:
	static BuddyChatManager * instance();

	Chat buddyChat(const Chat &chat);
	Chat buddyChat(const BuddySet &buddies);

};

/**
 * @}
 */

#endif // BUDDY_CHAT_MANAGER_H
