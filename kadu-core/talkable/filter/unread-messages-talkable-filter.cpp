/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"

#include "unread-messages-talkable-filter.h"

UnreadMessagesTalkableFilter::UnreadMessagesTalkableFilter(QObject *parent) :
		TalkableFilter(parent)
{
}

UnreadMessagesTalkableFilter::~UnreadMessagesTalkableFilter()
{
}

TalkableFilter::FilterResult UnreadMessagesTalkableFilter::filterChat(const Chat &chat)
{
	if (chat.unreadMessagesCount() > 0)
		return Accepted;
	else
		return Undecided;
}

TalkableFilter::FilterResult UnreadMessagesTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (buddy.unreadMessagesCount() > 0)
		return Accepted;
	else
		return Undecided;
}

TalkableFilter::FilterResult UnreadMessagesTalkableFilter::filterContact(const Contact &contact)
{
	if (contact.unreadMessagesCount() > 0)
		return Accepted;
	else
		return Undecided;
}

#include "moc_unread-messages-talkable-filter.cpp"
