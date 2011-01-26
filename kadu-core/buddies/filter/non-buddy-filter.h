/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NON_BUDDY_FILTER_H
#define NON_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "buddies/buddy.h"

#include "abstract-buddy-filter.h"

class NonBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Buddy CurrentBuddy;

public:
	explicit NonBuddyFilter(QObject *parent = 0);
	virtual ~NonBuddyFilter();

	void setBuddy(Buddy buddy);
	virtual bool acceptBuddy(const Buddy &buddy);

};

Q_DECLARE_METATYPE(NonBuddyFilter *)

#endif // NON_BUDDY_FILTER_H
