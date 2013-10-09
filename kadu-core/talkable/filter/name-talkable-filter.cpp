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
#include "buddies/group.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#include "name-talkable-filter.h"

NameTalkableFilter::NameTalkableFilter(NameFilterMatchingMode mode, QObject *parent) :
		TalkableFilter(parent), Mode(mode)
{
}

NameTalkableFilter::~NameTalkableFilter()
{
}

bool NameTalkableFilter::matches(const Chat &chat)
{
	if (!chat)
		return false;

	if (chat.display().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (const Buddy &buddy, chat.contacts().toBuddySet())
		if (matches(buddy))
			return true;

	foreach (const Group &group, chat.groups())
		if (group.name().contains(Name, Qt::CaseInsensitive))
			return true;

	return false;
}

bool NameTalkableFilter::matches(const Buddy &buddy)
{
	if (!buddy)
		return false;
	if (buddy.display().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.firstName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.lastName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.nickName().contains(Name, Qt::CaseInsensitive))
		return true;
	if (buddy.email().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (const Contact &contact, buddy.contacts())
		if (matches(contact))
			return true;

	foreach (const Group &group, buddy.groups())
		if (group.name().contains(Name, Qt::CaseInsensitive))
			return true;

	return false;
}

bool NameTalkableFilter::matches(const Contact &contact)
{
	return contact.id().contains(Name, Qt::CaseInsensitive);
}

TalkableFilter::FilterResult NameTalkableFilter::computeResult(bool matched)
{
	switch (Mode)
	{
		case AcceptMatching:
			return matched ? Accepted : Rejected;
		case UndecidedMatching:
			return matched ? Undecided : Rejected;
	}

	Q_ASSERT(false);

	return Undecided;
}

TalkableFilter::FilterResult NameTalkableFilter::filterChat(const Chat &chat)
{
	if (Name.isEmpty())
		return Undecided;

	return computeResult(matches(chat));
}

TalkableFilter::FilterResult NameTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (Name.isEmpty())
		return Undecided;

	return computeResult(matches(buddy));
}

TalkableFilter::FilterResult NameTalkableFilter::filterContact(const Contact &contact)
{
	if (Name.isEmpty())
		return Undecided;

	return computeResult(matches(contact.ownerBuddy()));
}

void NameTalkableFilter::setName(const QString &name)
{
	if (Name == name)
		return;

	Name = name;
	emit filterChanged();
}

#include "moc_name-talkable-filter.cpp"
