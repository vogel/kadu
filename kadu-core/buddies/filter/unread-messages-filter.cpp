/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/message/message-manager.h"

#include "unread-messages-filter.h"

UnreadMessagesFilter::UnreadMessagesFilter(QObject *parent) :
		AbstractBuddyFilter(parent)
{
	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
			this, SIGNAL(filterChanged()));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
			this, SIGNAL(filterChanged()));
}

UnreadMessagesFilter::~UnreadMessagesFilter()
{
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
			this, SIGNAL(filterChanged()));
	disconnect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
			this, SIGNAL(filterChanged()));
}

bool UnreadMessagesFilter::acceptBuddy(const Buddy &buddy)
{
	Q_UNUSED(buddy)
	return true;
}

bool UnreadMessagesFilter::ignoreNextFilters(const Buddy &buddy)
{
	return buddy.unreadMessagesCount() > 0;
}
