/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "buddies/buddy-set.h"
#include "buddies/buddy.h"

#include "blocked-buddy-filter.h"

BlockedBuddyFilter::BlockedBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent), Enabled(false)
{
}

BlockedBuddyFilter::~BlockedBuddyFilter()
{
}

void BlockedBuddyFilter::setEnabled(bool enabled)
{
	if (enabled == Enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

bool BlockedBuddyFilter::acceptBuddy(const Buddy &buddy)
{
	if (!Enabled)
		return true;

	return !buddy.isBlocked();
}
