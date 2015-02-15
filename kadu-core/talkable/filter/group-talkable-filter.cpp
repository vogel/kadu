/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
		TalkableFilter(parent)
{
}

GroupTalkableFilter::~GroupTalkableFilter()
{
}

bool GroupTalkableFilter::acceptGroupList(const QSet<Group> &groups, bool showInEverybodyGroup)
{
	switch (CurrentGroupFilter.filterType())
	{
		case GroupFilterInvalid:
			return true;
		case GroupFilterRegular:
			return groups.contains(CurrentGroupFilter.group());
		case GroupFilterEverybody:
			return showInEverybodyGroup;
		case GroupFilterUngroupped:
			return groups.isEmpty();
	}

	return false;
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

void GroupTalkableFilter::setGroupFilter(const GroupFilter &groupFilter)
{
	if (CurrentGroupFilter == groupFilter)
		return;

	CurrentGroupFilter = groupFilter;
	emit filterChanged();
}

#include "moc_group-talkable-filter.cpp"
