/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include "blocked-talkable-filter.h"

BlockedTalkableFilter::BlockedTalkableFilter(QObject *parent) :
		TalkableFilter(parent), Enabled(false)
{
}

BlockedTalkableFilter::~BlockedTalkableFilter()
{
}

TalkableFilter::FilterResult BlockedTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (!Enabled)
		return Undecided;

	if (buddy.isBlocked())
		return Rejected;
	else
		return Undecided;
}

TalkableFilter::FilterResult BlockedTalkableFilter::filterContact(const Contact &contact)
{
	if (!Enabled)
		return Undecided;

	if (contact.ownerBuddy().isBlocked())
		return Rejected;
	else
		return Undecided;
}

void BlockedTalkableFilter::setEnabled(bool enabled)
{
	if (Enabled == enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

#include "moc_blocked-talkable-filter.cpp"
