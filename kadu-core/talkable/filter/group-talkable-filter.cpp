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

#include "buddies/buddy.h"
#include "chat/chat.h"

#include "group-talkable-filter.h"

GroupTalkableFilter::GroupTalkableFilter(QObject *parent) :
		TalkableFilter(parent), AllGroupShown(true)
{
}

GroupTalkableFilter::~GroupTalkableFilter()
{
}

bool GroupTalkableFilter::acceptGroupList(const QList<Group> &groups, bool showInAllGroup)
{
	if (CurrentGroup)
		return groups.contains(CurrentGroup);
	if (AllGroupShown)
		return showInAllGroup;
	return groups.isEmpty();
}

TalkableFilter::FilterResult GroupTalkableFilter::filterChat(const Chat &chat)
{
	if (acceptGroupList(chat.groups(), chat.showInAllGroup()))
		return Undecided;
	else
		return Rejected;
}

TalkableFilter::FilterResult GroupTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (acceptGroupList(buddy.groups(), buddy.showInAllGroup()))
		return Undecided;
	else
		return Rejected;
}

void GroupTalkableFilter::setGroup(const Group &group)
{
	if (CurrentGroup == group)
		return;

	CurrentGroup = group;
	emit filterChanged();
}

void GroupTalkableFilter::setAllGroupShown(bool shown)
{
	if (AllGroupShown == shown)
		return;

	AllGroupShown = shown;
	emit filterChanged();
}
