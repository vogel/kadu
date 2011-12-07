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

#ifndef GROUP_TALKABLE_FILTER_H
#define GROUP_TALKABLE_FILTER_H

#include "buddies/group.h"

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class GroupTalkableFilter
 * @author Rafał 'Vogel' Malinowski
 * @short Filter that removes items that do not belong to specific group.
 *
 * This filter removes items that do not belong to specific group. All contacts are passed
 * to next filters. All buddies and chats that belong to specifis group are passed to next
 * filters.
 *
 * This filter is configured by two parameters: CurrentGroup and AllGroupShown. If AllGroupShown
 * is set to false and CurrentGroup is null, then this filter will reject all items that are
 * assigned to at least one group - that allows displaying "ungroupped" items on roster widget.
 */
class GroupTalkableFilter : public TalkableFilter
{
	Q_OBJECT

	Group CurrentGroup;
	bool AllGroupShown;

	bool acceptGroupList(const QList<Group> &groups, bool showInAllGroup);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of GroupTalkableFilter with given parent.
	 * @param parent QObject parent of new object
	 */
	explicit GroupTalkableFilter(QObject *parent = 0);
	virtual ~GroupTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);
	virtual FilterResult filterBuddy(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Change AllGroupShown paremeter value.
	 * @param shown new value of AllGroupShown parameter
	 *
	 * If AllGroupShown is true then for empty CurrentGroup all items (with showInAllGroup attribute set
	 * to true) will be dispalyed. If AllGroupShown is false then for empty CurrentGroup all items without
	 * any group will be displayed.
	 *
	 * If new value of parameter is different from old one then filterChanged() signal will be emited.
	 */
	void setAllGroupShown(bool shown);

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Change currently filtered group.
	 * @param group new group to fitler
	 *
	 * Only items belonging to this new group will be displyed. If AllGroupShown is set to false and group
	 * is null then all item without any group will be displayed.
	 *
	 * If new group is not equal to old one then filterChanged() signal will be emited.
	 */
	void setGroup(const Group &group);

};

/**
 * @}
 */

#endif // GROUP_TALKABLE_FILTER_H
