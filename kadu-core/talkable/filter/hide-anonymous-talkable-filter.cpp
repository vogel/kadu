/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hide-anonymous-talkable-filter.h"

HideAnonymousTalkableFilter::HideAnonymousTalkableFilter(QObject *parent) :
		TalkableFilter(parent), Enabled(true)
{
}

HideAnonymousTalkableFilter::~HideAnonymousTalkableFilter()
{
}

void HideAnonymousTalkableFilter::setEnabled(bool enabled)
{
	if (Enabled == enabled)
		return;

	Enabled = enabled;
	emit filterChanged();
}

TalkableFilter::FilterResult HideAnonymousTalkableFilter::filterChat(const Chat &chat)
{
	if (!Enabled)
		return Undecided;

	if (chat.display().isEmpty())
		return Rejected;
	else
		return Undecided;
}

TalkableFilter::FilterResult HideAnonymousTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (!Enabled)
		return Undecided;

	if (buddy.isAnonymous())
		return Rejected;
	else
		return Undecided;
}

TalkableFilter::FilterResult HideAnonymousTalkableFilter::filterContact(const Contact &contact)
{
	if (!Enabled)
		return Undecided;

	if (contact.isAnonymous())
		return Rejected;
	else
		return Undecided;
}

#include "moc_hide-anonymous-talkable-filter.cpp"
