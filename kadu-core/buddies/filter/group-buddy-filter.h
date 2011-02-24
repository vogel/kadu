/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GROUP_BUDDY_FILTER
#define GROUP_BUDDY_FILTER

#include "buddies/group.h"

#include "abstract-buddy-filter.h"

class GroupBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Group CurrentGroup;
	bool AllGroupShown;

public:
	GroupBuddyFilter(QObject *parent = 0);

	void setGroup(Group group);
	virtual bool acceptBuddy(const Buddy &buddy);

	void refresh();

	void setAllGroupShown(bool shown);

};

#endif // GROUP_BUDDY_FILTER
