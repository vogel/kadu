/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include "buddies/group-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"

#include "group-chat-filter.h"

GroupChatFilter::GroupChatFilter(QObject *parent) :
		ChatFilter(parent), CurrentGroup(Group::null), AllGroupShown(true)
{
	// for Chat::showInAllGroup()
	connect(GroupManager::instance(), SIGNAL(groupUpdated(Group)),
			this, SIGNAL(filterChanged()));
}

void GroupChatFilter::setGroup(const Group &group)
{
	if (CurrentGroup == group)
		return;

	CurrentGroup = group;
	emit filterChanged();
}

bool GroupChatFilter::acceptChat(const Chat &chat)
{
	return CurrentGroup
			? chat.isInGroup(CurrentGroup)
			: AllGroupShown
				? chat.showInAllGroup()
				: chat.groups().isEmpty();
}

void GroupChatFilter::setAllGroupShown(bool shown)
{
	if (AllGroupShown == shown)
		return;

	AllGroupShown = shown;
	emit filterChanged();
}
