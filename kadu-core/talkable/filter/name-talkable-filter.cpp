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
#include "contacts/contact.h"
#include "contacts/contact-set.h"

#include "name-talkable-filter.h"

NameTalkableFilter::NameTalkableFilter(QObject *parent) :
		TalkableFilter(parent)
{
}

NameTalkableFilter::~NameTalkableFilter()
{
}

TalkableFilter::FilterResult NameTalkableFilter::filterChat(const Chat &chat)
{
	if (Name.isEmpty())
		return Undecided;

	if (chat.display().contains(Name, Qt::CaseInsensitive))
		return Accepted;

	foreach (const Buddy &buddy, chat.contacts().toBuddySet())
		if (Accepted == filterBuddy(buddy))
			return Accepted;

	return Rejected;
}

TalkableFilter::FilterResult NameTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (Name.isEmpty())
		return Undecided;

	if (buddy.display().contains(Name, Qt::CaseInsensitive))
		return Accepted;
	if (buddy.firstName().contains(Name, Qt::CaseInsensitive))
		return Accepted;
	if (buddy.lastName().contains(Name, Qt::CaseInsensitive))
		return Accepted;
	if (buddy.nickName().contains(Name, Qt::CaseInsensitive))
		return Accepted;
	if (buddy.email().contains(Name, Qt::CaseInsensitive))
		return Accepted;

	foreach (const Contact &contact, buddy.contacts())
		if (Accepted == filterContact(contact))
			return Accepted;

	return Rejected;
}

TalkableFilter::FilterResult NameTalkableFilter::filterContact(const Contact &contact)
{
	if (Name.isEmpty())
		return Undecided;

	if (contact.id().contains(Name, Qt::CaseInsensitive))
		return Accepted;
	else
		return Rejected;
}

void NameTalkableFilter::setName(const QString &name)
{
	if (Name == name)
		return;

	Name = name;
	emit filterChanged();
}
