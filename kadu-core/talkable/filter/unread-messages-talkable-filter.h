/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef UNREAD_MESSAGES_TALKABLE_FILTER_H
#define UNREAD_MESSAGES_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class UnreadMessagesTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that accepts all item that have unread messages.
 *
 * This filter accepts all item that have unread messages. Items withoud unread messages are passed to
 * next filters.
 */
class UnreadMessagesTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of UnreadMessagesTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit UnreadMessagesTalkableFilter(QObject *parent = 0);
	virtual ~UnreadMessagesTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);
	virtual FilterResult filterBuddy(const Buddy &buddy);
	virtual FilterResult filterContact(const Contact &contact);

};

/**
 * @}
 */

#endif // UNREAD_MESSAGES_TALKABLE_FILTER_H
