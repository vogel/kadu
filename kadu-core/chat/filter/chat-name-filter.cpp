/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/buddy-list.h"
#include "contacts/contact-set.h"

#include "chat-name-filter.h"

ChatNameFilter::ChatNameFilter(QObject *parent) :
		ChatFilter(parent)
{
	Filter = new BuddyNameFilter(this);
}

bool ChatNameFilter::acceptChat(Chat chat)
{
	if (Name.isEmpty())
		return true;

	if (chat.name().contains(Name, Qt::CaseInsensitive))
		return true;

	foreach (const Buddy &buddy, chat.contacts().toBuddySet())
		if (Filter->acceptBuddy(buddy))
			return true;

	return false;
}

void ChatNameFilter::setName(const QString& name)
{
	if (name != Name)
	{
		Name = name;
		Filter->setName(Name);
		emit filterChanged();
	}
}
