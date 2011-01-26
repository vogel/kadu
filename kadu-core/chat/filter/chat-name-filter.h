/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CHAT_NAME_FILTER_H
#define CHAT_NAME_FILTER_H

#include "chat/filter/chat-filter.h"
#include "exports.h"

class BuddyNameFilter;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatNameFilter
 * @short Chat filter that filters chats by names of buddies and ids of contacts.
 *
 * This ChatFilter class filters chats by names (using Chat::name() method) and by
 * names of buddies in this chat (using BuddyNameFilter class on every buddy).
 */
class KADUAPI ChatNameFilter : public ChatFilter
{
	Q_OBJECT

	QString Name;
	BuddyNameFilter *Filter;

public:
	ChatNameFilter(QObject *parent = 0);

	virtual bool acceptChat(const Chat &chat);

	void setName(const QString &name);

};

/**
* @}
*/

#endif // CHAT_NAME_FILTER_H
