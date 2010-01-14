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
	QString Name;
	BuddyNameFilter *Filter;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Contructs empty ChatNameFilter object that accepts all chats.
	 * @param parent parent QObject
	 *
	 * Contructs empty ChatNameFilter object that accepts all chats.
	 */
	ChatNameFilter(QObject *parent = 0);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if chat name (or its buddies names) matches filter's name.
	 * @param chat Chat object that will be checked
	 * @return true if chat name (or its buddies names) matches filter's name
	 *
	 * Return true if chat name (or its buddies names) matches filter's name.
	 * For checking buddies name BuddyNameFilter object is used on every
	 * buddy in this chat.
	 */
	virtual bool acceptChat(Chat chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Changes name of this filter.
	 *
	 * Sets new name to filter chats. If new name is different that old one
	 * filterChanged signal is emited and models can be invalidated.
	 */
	void setName(const QString &name);

};

/**
* @}
*/

#endif // CHAT_NAME_FILTER_H
